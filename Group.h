#pragma once

#include<map>
#include<string>
#include<vector>
#include"Stock.h"

class Group {
private:
	std::map<std::string, Stock> stocks;
	std::vector<std::string> map_keys;
	std::vector<double> AAR = std::vector<double>(91);
	std::vector<double> CAAR = std::vector<double>(91);
public:
	Group();
	Group(const Group& g_p); //Realize the copy constructor
	~Group();
	const std::vector<double>& GetAAR();
	const std::vector<double>& GetCAAR();
	const Stock& GetStock(std::string ticker) const;
	const std::vector<std::string>& GetMapKeys() const;
	const int GetSize() const;
	bool PushStock(const Stock& s);
	bool Compute(); // wrong calculation
};

