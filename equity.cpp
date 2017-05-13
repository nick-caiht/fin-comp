#include "curl.h"
#include "date.h"
#include "equity.h"
#include <iostream>
#include <map>

Equity::Equity(std::string ticker_, Date s, Date e, std::map<Date, double> price_, std::map<Date, double> ret_) 
	:ticker(ticker_), start_date(s), end_date(e), price(price_), ret(ret_) {
}

const Date& Equity::GetStartDate() const {
	return start_date;
}

const Date& Equity::GetEndDate() const {
	return end_date;
}

std::string Equity::GetTicker() const {
	return ticker;
}

const std::map<Date, double>& Equity::GetPrice() const {
	return price;
}

const std::map<Date, double>& Equity::GetReturn() const {
	return ret;
}

void Equity::display()  {
	std::cout << "Ticker: " << ticker << std::endl;
	std::cout << "price at start: " << price.begin()->second << std::endl << "price at end: " << (--price.end())->second << std::endl;
	std::cout << "size: " << price.size() << ", " << ret.size() << std::endl;
	std::cout << "start date: " << start_date << std::endl;
	std::cout << "end date:" << end_date << std::endl;
}

const Date& Stock::GetRepDate() {
	return (*this).rep_date;
}

void Stock::display() {
	Equity::display();
	std::cout << "report date: " << rep_date << std::endl;
	std::cout << "surprise: " << surprise << std::endl;
	std::cout << "security: " << security << std::endl;
	std::cout << "sector: " << sector << std::endl;
	std::cout << "sub industry: " << sub_industry << std::endl;
	std::cout << "address: " << address << std::endl;
	std::cout << "cik: " << cik << std::endl << std::endl;
}


ETF ETF::Slice(Date s_d, Date e_d) const {
	std::map<Date, double> subprice;
	std::map<Date, double> subret;
	for (auto i = price.find(s_d); i != ++(price.find(e_d)); i++) {
		subprice[i->first] = i->second;
	}
	for (auto i = ret.find(s_d); i != ++(ret.find(e_d)); i++) {
		subret[i->first] = i->second;
	}
	return ETF(ticker, s_d, e_d, subprice, subret);
}

Stock::Stock(std::string ticker_, Date s, Date e, Date report,
	std::map<Date, double> price_, std::map<Date, double> ret_,
	std::string security_, std::string sector_, std::string subsector_,
	std::string address_, std::string cik_, double surprise_)
	:Equity(ticker_, s, e, price_, ret_), rep_date(report),
	security(security_), sector(sector_), sub_industry(subsector_),
	address(address_), cik(cik_), surprise(surprise_) {
}

Stock::Stock(const Stock& stock) : rep_date(stock.rep_date), security(stock.security), sector(stock.sector), sub_industry(stock.sub_industry), address(stock.address), cik(stock.cik), surprise(stock.surprise) {
	Equity::ticker = stock.ticker;
	Equity::start_date = stock.start_date;
	Equity::end_date = stock.end_date;
	Equity::price = stock.price;
	Equity::ret = stock.ret;
}