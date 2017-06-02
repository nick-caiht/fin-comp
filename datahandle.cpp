#include "curl.h"
#include "datahandle.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <stack>

/*Generate a url*/
std::string url_generator(std::string ticker, Date rp_date) {
	std::stringstream ss;
	std::string base_("http://real-chart.finance.yahoo.com/table.csv?");
	if (ticker != "SPY") {
		if (rp_date.GetMonth() <= 1)
			ss << base_ << "s=" << ticker << "&a=" << std::to_string(rp_date.GetMonth() + 10) << "&b=" << "1" << "&c=" << std::to_string(rp_date.GetYear() - 1) << "&d=" << std::to_string(rp_date.GetMonth() + 4) << "&e=" << "28" << "&f=" << std::to_string(rp_date.GetYear()) << "&g=d&ignore=.csv";
		else if (rp_date.GetMonth() >= 2 && rp_date.GetMonth() <= 7)
			ss << base_ << "s=" << ticker << "&a=" << std::to_string(rp_date.GetMonth() - 2) << "&b=" << "1" << "&c=" << std::to_string(rp_date.GetYear()) << "&d=" << std::to_string(rp_date.GetMonth() + 4) << "&e=" << "28" << "&f=" << std::to_string(rp_date.GetYear()) << "&g=d&ignore=.csv";
		else //month 8~11
			ss << base_ << "s=" << ticker << "&a=" << std::to_string(rp_date.GetMonth() - 2) << "&b=" << "1" << "&c=" << std::to_string(rp_date.GetYear()) << "&d=" << std::to_string(rp_date.GetMonth() - 8) << "&e=" << "28" << "&f=" << std::to_string(rp_date.GetYear() + 1) << "&g=d&ignore=.csv";
		std::string s = ss.str();
	} else {
		//SPY: from 2015-8-1 to 2017-5-1
		ss << base_ << "s=" << ticker << "&a=7&b=1&c=2015&d=4&e=1&f=2017&g=d&ignore=.csv";
	}
	return ss.str();
}

struct MemoryStruct {
	char *memory;
	size_t size;
};

void *myrealloc(void *ptr, size_t size) {
	/* There might be a realloc() out there that doesn't like reallocing
	NULL pointers, so we take care of it here */
	if (ptr)
		return realloc(ptr, size);
	else
		return malloc(size);
}

/* Write data to memory */
int write_data2(void *ptr, size_t size, size_t nmemb, void *data) {
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)data;

	mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory) {
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}
	return realsize;
}

/* Download, process prices and calculate returns */
void PriceHandler(std::string ticker, std::string url, Date report_date, Date& start_date, Date& end_date, std::map<Date, double>& price, std::map<Date, double>& ret) {
	struct MemoryStruct readBuffer;
	readBuffer.memory = NULL;
	readBuffer.size = 0;
	std::stack<Date> dates;
	std::stack<double> adj;

	//instantiate and initialize CURL
	CURL *handle;
	CURLcode result;
	handle = curl_easy_init();
	//if handle is good, perform
	if (handle) {
		curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
		curl_easy_setopt(handle, CURLOPT_ENCODING, "");
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&readBuffer);
		result = curl_easy_perform(handle);
		// Check for errors 
		if (result != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
		} else {
			int count_push = 0;
			int count_pop = 0;

			std::istringstream ss(readBuffer.memory);
			std::string token;
			getline(ss, token, '\n');
			//read memory by line
			while (getline(ss, token, '\n')) {
				//parse string to fields
				std::string temp = token.substr(0, 10);
				int cur_year = stoi(temp.substr(0, 4));
				int cur_month = stoi(temp.substr(5, 6));
				int cur_day = stoi(temp.substr(8, 9));
				Date cur(cur_year, cur_month, cur_day);
				if (ticker == "SPY") {
					dates.push(cur);
					adj.push(stod(token.substr(token.find_last_of(',') + 1)));
				} else {
					//keep pushing until cur < report date, then push 30 more
					if (cur >= report_date) {
						dates.push(cur);
						adj.push(stod(token.substr(token.find_last_of(',') + 1)));
					} else {
						dates.push(cur);
						adj.push(stod(token.substr(token.find_last_of(',') + 1)));
						count_push++;
					}
					if (count_push == 30) {
						//std::cout << "30 pushed" << std::endl;
						break;
					}
				}
			}
			if (ticker == "SPY") {
				while (!dates.empty()) {
					price[dates.top()] = adj.top();
					dates.pop();
					adj.pop();
				}
			} else {
				//pop and save reversed price into map
				while (!dates.empty()) {
					if (dates.top() <= report_date) {
						price[dates.top()] = adj.top();
						dates.pop();
						adj.pop();
					} else {
						price[dates.top()] = adj.top();
						dates.pop();
						adj.pop();
						count_pop++;
					}
					if (count_pop == 60) {
						//std::cout << "60 poped" << std::endl;
						break;
					}
				}
			}
			//modify start_date & end_date
			start_date = (price.begin())->first;
			end_date = (--price.end())->first;
			//calcualte returns in percent and save to map
			for (auto i = price.begin(); i != --price.end(); i++) {
				auto j = i;
				j++;
				ret[j->first] = (j->second - i->second) / i->second * 100;
			}
		}
	} else {
		fprintf(stderr, "Curl init failed!\n");
	}
	//clean up
	curl_easy_cleanup(handle);
}

/* Multithread Downloading */
void threadDownload(std::string line, std::map<std::string, Stock>& all_stocks) {
	std::string ticker;
	std::string date;
	double surp;
	int y;
	int m;
	int d;
	std::string sec_name;
	std::string comp_sector;
	std::string comp_subsec;
	std::string addr;
	std::string cik_name;
	/* cannot use file.good() or !file.eof() because cannot find eof on last line
	Using getline to parse line	and store info. */
	std::istringstream iss(line);
	std::string token;
	getline(iss, token, ',');
	ticker = token;
	getline(iss, token, ',');
	date = token;
	getline(iss, token, ',');
	surp = stod(token);
	getline(iss, token, ',');
	y = stoi(token);
	getline(iss, token, ',');
	m = stoi(token);
	getline(iss, token, ',');
	d = stoi(token);
	getline(iss, token, ',');
	sec_name = token;
	getline(iss, token, ',');
	comp_sector = token;
	getline(iss, token, ',');
	comp_subsec = token;
	getline(iss, token, ',');
	addr = token;
	getline(iss, token, ',');
	cik_name = token;

	//Declare containers to store data
	std::map<Date, double> cur_price, cur_ret;
	Date cur_start, cur_end;
	//generate a url
	Date url_rp_date(y, m, d);
	std::string cur_url = url_generator(ticker, url_rp_date);
	//download prices and calculate returns
	Date cur_rep_date(y, m + 1, d);
	PriceHandler(ticker, cur_url, cur_rep_date, cur_start, cur_end, cur_price, cur_ret);
	//stock constructor with parameter
	Stock cur_stock(ticker, cur_start, cur_end, cur_rep_date, cur_price, cur_ret, sec_name,
		comp_sector, comp_subsec, addr, cik_name, surp);
	
	//NOTICE: race condition!
	//check if mutex has been released 
	std::lock_guard<std::mutex> block_threads_until_finish_this(barrier);
	//save stock into stock pool
	all_stocks[ticker] = cur_stock;
	std::cout << ticker << std::endl; //print downloaded ticker for debug 
}

void AllStock(std::map<std::string, Stock>& all_stocks, ETF& SPY, QWidget *w) {
	int download_count = 0;
	int download_max = 454;	
	//stock pool. with infomation listed below.
	std::ifstream file("sur.csv");
	std::string line;
	//skip header
	getline(file, line);
	//QProgressDialog process("Downloading", "Cancel", download_count, download_max, w);
	//process.setWindowModality(Qt::WindowModal);
	std::vector<std::thread> mythread;
	while (getline(file, line)) {
		//fork a new thread to download data.
		//To learn more about r-value reference, see http://thbecker.net/articles/rvalue_references/section_01.html 
		mythread.push_back(std::thread(threadDownload, line, std::ref(all_stocks)));
		download_count++;		
		//Process Bar
		/*
		if (process.wasCanceled()) {
			break;
		}
		process.setValue(download_count);
		QApplication::processEvents();*/
	}
	file.close();
	
	//dowaload SPY
	std::map<Date, double> spy_price, spy_ret;
	std::string spy = "SPY";
	Date spy_start, spy_end;
	PriceHandler(spy, url_generator(spy, Date()), Date(), spy_start, spy_end, spy_price, spy_ret);
	SPY = ETF(spy, spy_start, spy_end, spy_price, spy_ret);

	//join the threads
	for (auto &t : mythread) {
		t.join();
	}
	std::cout << "Total downloaded stocks: " << all_stocks.size() << std::endl;
}
