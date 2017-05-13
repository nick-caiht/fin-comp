#include "group.h"
#include <vector>

Group::Group() {
}

Group::~Group() {
}

Group::Group(const Group& g_p) :stocks(g_p.stocks), map_keys(g_p.map_keys), AAR(g_p.AAR), CAAR(g_p.CAAR) {
}

const std::vector<double>& Group::GetAAR() const {
	return AAR;
}

const std::vector<double>& Group::GetCAAR() const {
	return CAAR;
}

bool Group::PushStock(const Stock& s) {
	stocks.insert(std::pair<std::string, Stock>(s.GetTicker(), s));
	map_keys.push_back(s.GetTicker());
	return true;
}

const Stock& Group::GetStock(std::string ticker) const {
	auto i = stocks.find(ticker);
	return i->second;
}

/* Compute AAR & CAAR */
bool Group::Compute(const ETF& SPY) {
	//for each stock
	int count = 0;
	for (auto i = stocks.begin(); i != stocks.end(); ++i) {
		//Slice a sub-SPY with corresponding dates
		ETF slicedSPY = SPY.Slice(i->second.GetStartDate(), i->second.GetEndDate());
		auto j = i->second.GetReturn().begin();
		for (int t = 0; j != i->second.GetReturn().end(); ++j, ++t) {
			double AR = j->second - slicedSPY.GetReturn().find(j->first)->second;
			//Calc running AAR
			AAR[t] = (AAR[t] * count + AR) / (count+1);
		}
		++count;
	}
	//Calc CAAR
	CAAR[0] = AAR[0];
	for (int i = 1; i < 90; ++i) {
		CAAR[i] = CAAR[i - 1] + AAR[i];
	}
	return true;
}

const int Group::GetSize() const {
	return map_keys.size();
}

const std::vector<std::string>& Group::GetMapKeys() const {
	return map_keys;
}
