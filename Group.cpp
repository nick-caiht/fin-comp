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
	} else {
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

bool Group::Compute(const ETF& SPY) {
	//for each stock
	for (int count = 0, auto i = stocks.begin(); i != stocks.end(); ++i, ++count) {
		//Slice a sub-SPY with corresponding dates
		ETF slicedSPY = SPY.Slice(i->second.start_date, i->second.end_date);
		if(slicedSPY.re_turn.size() != 91 || i->second.re_turn.size() != 91) {
			return false;
		}
		auto j = i->second.re_turn.begin();
		for (int t = 0; j != i->second.re_turn.end(); ++j, ++t) {
			double AR = j->second - slicedSPY.re_turn.find(j->first)->second;
			//Calc running AAR
			AAR[t] = (AAR[t] * count + AR) / (count+1);
		}
	}
	//Calc CAAR
	CAAR[0] = AAR[0];
	for (int i = 1; i < 91; ++i) {
		CAAR[i] = CAAR[i - 1] + AAR[i];
	}
}

const int Group::GetSize() const {
	return map_keys.size();
}

const std::vector<std::string>& Group::GetMapKeys() const {
	return map_keys;
}
