#pragma once

#include<map>
#include<string>
#include<vector>
#include"equity.h"

class Group {
private:
	std::multimap<std::string, Stock> stocks;
	std::vector<std::string> map_keys;
	std::vector<double> AAR = std::vector<double>(91);
	std::vector<double> CAAR = std::vector<double>(91);
public:
	Group();
	Group(const Group& g_p); 
	~Group();
	const std::vector<double>& GetAAR();
	const std::vector<double>& GetCAAR();
	const Stock& GetStock(std::string ticker) const;
	const std::vector<std::string>& GetMapKeys() const;
	const int GetSize() const;
	bool PushStock(const Stock& s);
	bool Compute(const ETF& SPY);
};
