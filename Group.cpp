#include "Group.h"
#include <vector>
Group::Group() {

}

Group::~Group() {
}

Group::Group(const Group& g_p) {
	stocks = g_p.stocks;
	map_keys = g_p.map_keys;
	AAR = g_p.AAR;
	CAAR = g_p.CAAR;
}

const std::vector<double>& Group::GetAAR() {
	return AAR;
}

const std::vector<double>& Group::GetCAAR() {
	return CAAR;
}

bool Group::PushStock(const Stock& s) {
	auto i = stocks.find(s.getTicker());
	if (i != stocks.end()) {
		stocks[s.getTicker()] = s;
		map_keys.push_back(s.getTicker());
		return true;
	}
	else {
		return false;
	}
}

const Stock& Group::GetStock(std::string ticker) const {
	auto i = stocks.find(ticker);
	if (i != stocks.end()) {
		return i->second;
	}
	//future improvment: try catch
}

//how to get ETF? pass by reference? Compute(ETF &SPY) 
bool Group::Compute() {
	std::vector<std::vector<double>> AR; //store abnormal returns
	AR.resize(stocks.size()); 
	for (int i = 0; i < stocks.size(); i++) {
		AR[i].resize(90);
	}
	int stockCount = 0;
	for (auto i = stocks.begin(); i != stocks.end(); i++){ //for each stock
		ETF slicedSPY = SPY.Slice(i->second.start_date, i->second.start_date);//SPY with corresponding dates
		auto slicedPtr = slicedSPY.re_turn.begin();
		int posCount = 0;
		for (auto j = i->second.re_turn.begin(); j != i->second.re_turn.end(); j++) { 
			AR[stockCount][posCount] = i->second.re_turn->second - slicedPtr->re_turn->second;//ARit
			slicedPtr++;
			posCount++;
		}
		stockCount++;
	}
	//calc AAR
	//calc CAAR
}

const int Group::GetSize() const {
	return map_keys.size();
}

const std::vector<std::string>& Group::GetMapKeys() const {
	return map_keys;
}
