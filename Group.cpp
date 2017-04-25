#include "Group.h"
#include <vector>
Group::Group() {

}

Group::~Group() {
}

Group::Group(const Group& g_p) :stocks(g_p.stocks), map_keys(g_p.map_keys), AAR(g_p.AAR), CAAR(g_p.CAAR){
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
	std::vector<double> AAR = std::vector<double>(90);
	for (auto i = stocks.begin(); i != stocks.end(); i++){ //for each stock
		ETF slicedSPY = SPY.Slice(i->second.start_date, i->second.start_date);//SPY with corresponding dates
		auto slicedPtr = slicedSPY.re_turn.begin();
		int posCount = 0;
		for (auto j = i->second.re_turn.begin(); j != i->second.re_turn.end(); j++) { 
			AAR[posCount] += (j->second - slicedPtr->second);//ARit
			slicedPtr++;
			posCount++;
		}
	}
	//calc AAR
	for(int i=0;i<90;i++)
	  AAR[i] = AAR[i] / stocks.size();
	//calc CAAR
	std::vector<double> CAAR = std::vector<double>(90);
	CAAR[0] = AAR[0];
	for (int i = 1; i < 90; i++)
		CAAR[i] = CAAR[i - 1] + AAR[i];
}

const int Group::GetSize() const {
	return map_keys.size();
}

const std::vector<std::string>& Group::GetMapKeys() const {
	return map_keys;
}
