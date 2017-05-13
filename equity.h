#pragma once
#include "curl.h"
#include "date.h"
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>

class Equity {
protected:
	std::string ticker;
	Date start_date;
	Date end_date;
	std::map<Date, double> price;
	std::map<Date, double> ret;

public:
	Equity() {}
	Equity(std::string ticker_, Date s, Date e, std::map<Date, double> price_, std::map<Date, double> ret_);
	Equity(const Equity& e):ticker(e.ticker), start_date(e.start_date), end_date(e.end_date), price(e.price), ret(e.ret){}
	virtual ~Equity() {}
	const Date& GetStartDate() const;
	const Date& GetEndDate() const;
	std::string GetTicker() const;
	const std::map<Date, double>& GetPrice() const;
	const std::map<Date, double>& GetReturn() const;
	virtual void display() ;
};

class ETF : public Equity {
public:
	ETF() {}
	ETF(std::string ticker_, Date s, Date e, std::map<Date, double> price_, std::map<Date, double> ret_) 
		:Equity(ticker_, s, e, price_, ret_) {}
	ETF(const ETF& etf) {
		Equity::ticker = etf.ticker;
		Equity::start_date = etf.start_date;
		Equity::end_date = etf.end_date;
		Equity::price = etf.price;
		Equity::ret = etf.ret;
	}
	ETF Slice(Date s_d, Date e_d) const;
};

class Stock :public Equity {
private:
	Date rep_date;
	double surprise;
	std::string security;
	std::string sector;
	std::string sub_industry;
	std::string address;
	std::string cik;

public:
	Stock() {}
	Stock(std::string ticker_, Date s, Date e, Date report,	std::map<Date, double> price_, std::map<Date, double> ret_,	std::string security_, std::string sector_, std::string subsector_,	std::string address_, std::string cik_, double surprise_);
	Stock(const Stock& stock);
	~Stock() {}
	const Date& GetRepDate();
	double GetSurprise() const { return surprise; }
	std::string GetSecurity() const { return security; }
	std::string GetSector() const { return sector; }
	std::string GetSubIndustry() const { return sub_industry; }
	std::string GetAddress() const { return address; }
	std::string GetCik() { return cik; }
	virtual void display();
};
