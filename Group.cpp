#include "Group.h"

Group::Group() {

}

Group::~Group() {
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

//wrong calculation. Notice lenth(SPY)>len(stock). Use ETF::Slice(startdate, enddate).
bool Group::Compute() {
  double re_turnSum = 0;
	for (auto j = stocks.begin(); j != stocks.end(); j++)
		re_turnSum += j->second.re_turn;
	AAR[0] = re_turnSum / stocks.size() - SPY.re_turn[i];
	CAAR[0] = AAR[0];
	for (int i = 1; i < AAR.size(); i++) {
		double re_turnSum = 0;
		for (auto j = stocks.begin(); j != stocks.end(); j++)
			re_turnSum += j->second.re_turn;
		AAR[i] = re_turnSum / stocks.size() - SPY.re_turn[i];
		CAAR[i] = CAAR[i - 1] + AAR[i];
	}
}

const int Group::GetSize() const {
	return map_keys.size();
}

const std::vector<std::string>& Group::GetMapKeys() const {
	return map_keys;
}
