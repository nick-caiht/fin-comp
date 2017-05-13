#pragma once

#include <QtWidgets/QWidget>
#include "ui_Project6883.h"
#include "curl.h"
#include "date.h"
#include "equity.h"
#include "datahandle.h"
#include "experiment.h"
#include "TickerNode.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <stack>


class Project6883 : public QWidget
{
	Q_OBJECT

public:
	Project6883(QWidget *parent = Q_NULLPTR);

private:
	Ui::Project6883Class ui;
	std::map<std::string, Stock> all_stocks;
	TickerNode all_tickers;
	ETF SPY;
	double threshold;
	int bootstraping_time;
	Experiment experiment;
	ExcelDriver *excel;
	int graph_count = 0;
	
	void addGroupTableElement(Group big_group);
	void drawGraph(int group);

private slots:
	void on_okButton_clicked(); //slot function for okButton click signal
	
	void on_excelButton_clicked(); //slot function for excelButton click signal
	
	void on_searchButton_clicked();//slot function for searchButton click signal
								   
	/*slot function for groupComboBox currentText Changed signals*/
	/*The signal function is: void currentTextChanged(const QString &text)*/
	void on_groupComboBox_currentTextChanged(const QString &text);

	/*slot function for double click cell in group table*/
	void on_groupTable_cellDoubleClicked(int row, int column);
	/*slot function for text changed in the stocklineEdit*/
	void on_radar_change(const QString &text);
	/*slot function for double click item in listWidget*/
	void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
};
