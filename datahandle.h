#pragma once

#include "curl.h"
#include "date.h"
#include "equity.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <stack>
#include <QObject>
#include <Qwidget>
#include <QProgressDialog>
#include <QApplication>
#include <thread>
#include <mutex>

static std::mutex barrier;

//class MyCount : public QObject{
//	Q_OBJECT
//private:
//	int number = 0;
//public:
//	void send() {
//		emit countSignal(number);
//	}
//};

std::string url_generator(std::string ticker, Date rp_date = Date());
void PriceHandler(std::string ticker, std::string url, Date report_date, Date& start_date, Date& end_date, std::map<Date, double>& price, std::map<Date, double>& ret);
void AllStock(std::map<std::string, Stock>& all_stocks, ETF& SPY, QWidget *w);
