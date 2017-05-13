#include "Project6883.h"
#include <iostream>
#include <string>
#include <vector>
#include <QtWidgets>

/*Constructor: set up ui and others*/
Project6883::Project6883(QWidget *parent) : QWidget(parent), all_tickers("root", false) {
	//initialize libcurl
	curl_global_init(CURL_GLOBAL_DEFAULT);
	excel = &(ExcelDriver::Instance());
	
	setFixedSize(1030, 729);
	ui.setupUi(this);
	ui.listWidget->setHidden(true);
	connect(ui.stockEdit, SIGNAL(textChanged(const QString &)), this, SLOT(on_radar_change(const QString &)));
	/*set up all lineEdit Widgets to use ENTER key*/
	connect(ui.stockEdit, SIGNAL(returnPressed()), ui.searchButton, SIGNAL(clicked()));
	connect(ui.thresholdEdit, SIGNAL(returnPressed()), ui.okButton, SIGNAL(clicked()));
	connect(ui.BootstrapingEdit, SIGNAL(returnPressed()), ui.okButton, SIGNAL(clicked()));
	//connect ok button with qprogressdialog
	//connect(ui.okButton, SIGNAL(clicked()), this, SLOT(start_process()));

	//downlaod data using libcurl
	AllStock(all_stocks, SPY, this);
	//insert all tickers into all_tickers, which is a Trie for ticker searching
	for (auto i = all_stocks.begin(); i != all_stocks.end(); i++) {
		all_tickers.insert(i->first, 0);
	}

	//cleanup libcurl
	curl_global_cleanup();

	/*set up stocksTable and groupTable*/
	int col_num = 4;//width of stocksTable
	int width[3] = { 125,130,130 };
	for (int i = 0; i < col_num; i++) {
		ui.stocksTable->setColumnWidth(i, width[i]);
	}
	ui.stocksTable->setRowCount(91);
	col_num = 2;//width of groupTable
	int width_g[2] = { 95,130 };
	for (int i = 0; i < col_num; i++) {
		ui.groupTable->setColumnWidth(i, width_g[i]);
	}
	ui.groupTable->setRowCount(0);
	//Use default threshold and bootstraping_number to calculate
	on_okButton_clicked();
}

/*slot function for okButton click signals*/
void Project6883::on_okButton_clicked() {
	//get inputs from two QlineEdit widgets
	threshold = ui.thresholdEdit->text().toDouble();
	bootstraping_time = ui.BootstrapingEdit->text().toInt();
	//std::cout << threshold << " " << bootstraping_time << std::endl;

	//check if inputs are valid
	if (threshold < 5 || threshold >10) {
		QMessageBox::critical(this, tr("Threshold out of range!"), tr("Threshold should be between 5% to 10%"));
		ui.thresholdEdit->setText(tr("10"));
		return;
	}
	if (bootstraping_time < 1) {
		ui.BootstrapingEdit->setText(tr("1"));
		QMessageBox::critical(this, tr("Bootstraping out of range!"), tr("It should be at least 1"));
		return;
	}
	if (bootstraping_time > 500) {
		ui.BootstrapingEdit->setText(tr("500"));
		bootstraping_time = ui.BootstrapingEdit->text().toInt();
		QMessageBox::information(this, tr("Bootstraping suggestion"), tr("It will take too long, just try 500^_^"));
	}

	//after checking inputs, do computations
	//and also update values of threshold an bootstraping_time in Event class
	experiment = Experiment(threshold, bootstraping_time);
	experiment.UpdateGroups(all_stocks);
	experiment.DoExperiment(SPY);
	QMessageBox::information(this, tr("Reset Groups"), tr("Calculation compeleted!"));

	//default set groupTable to empty
	ui.groupComboBox->setCurrentIndex(ui.groupComboBox->findText(tr("")));

	//drawGraph
	drawGraph(3);
}

/*slot function for excelButton click signals*/
void Project6883::on_excelButton_clicked() {
	//put data into excel 
	//and also draws graphs
	excel->MakeVisible(true);
	Vector<double, long> xarr(experiment.GetCAARlist().front().MaxIndex());
	//Vector<double, long> yarr(4);
	for (long i = xarr.MinIndex(); i <= xarr.MaxIndex(); i++) {
		xarr[i] = i - 30;
	}
	//printMatrixChartsInExcel(1, experiment.GetCAARmx(), xarr, yarr);
	list<string> labels;
	labels.push_back("Beat");
	labels.push_back("Meet");
	labels.push_back("Miss");
	graph_count++;
	std::string caar_title = "CAAR" + std::to_string(graph_count) + "(Threshold=" + std::to_string(threshold).substr(0, 5) + "%)";
	printInExcel(xarr, labels, experiment.GetCAARlist(), caar_title, std::string("Date"), std::string("Percent"));
	std::string aar_title = "AAR" + std::to_string(graph_count) + "(Threshold=" + std::to_string(threshold).substr(0, 5) + "%)";
	printInExcel(xarr, labels, experiment.GetAARlist(), aar_title, std::string("Date"), std::string("Percent"));
}

/*slot function for searchButton click signals*/
void Project6883::on_searchButton_clicked() {
	//set stockGroupLabel to show the stock's group
	QString ticker = ui.stockEdit->text();
	ticker = ticker.toUpper();

	if (ticker.isEmpty()) {//no input or no ticker
		ui.stocksTable->clearContents();
		ui.stockInformationList->clear();
	} else {
		auto stock_it = all_stocks.find(ticker.toStdString());
		if (stock_it == all_stocks.end()) {
			//no ticker
			ui.stocksTable->clearContents();
			ui.stockInformationList->clear();
			QMessageBox::information(this, tr("Error"), tr("No such Ticker!"));
			return;
		}
		Stock s = stock_it->second;
		//add data into stocksTable
		ui.stocksTable->clearContents();
		ui.stocksTable->setSortingEnabled(false);
		std::map<Date, double> price = s.GetPrice();
		std::map<Date, double> ret = s.GetReturn();
		int row_count = 0;
		//add information into stocksTable
		for (auto i = price.begin(); i != price.end(); i++) {
			//Dynamic allocation using smart pointers
			//The table owns the memory, which means that we don't have to deleting the pointer ourselves
			//column 0, Date
			QTableWidgetItem *item1 = new QTableWidgetItem(QString((i->first).toString().c_str()));
			item1->setFlags(item1->flags() ^ Qt::ItemIsEditable);
			ui.stocksTable->setItem(row_count, 0, item1);
			//column 1, price
			QTableWidgetItem *item2 = new QTableWidgetItem(QString(std::to_string(i->second).c_str()));
			item2->setFlags(item2->flags() ^ Qt::ItemIsEditable);
			ui.stocksTable->setItem(row_count, 1, item2);
			//column 2, return
			if (i != price.begin()) {
				double temp_return = ret.find(i->first)->second;
				QTableWidgetItem *item3 = new QTableWidgetItem(QString((std::to_string(temp_return).substr(0, 6) + "%").c_str()));
				item3->setFlags(item3->flags() ^ Qt::ItemIsEditable);
				ui.stocksTable->setItem(row_count, 2, item3);
			}
			row_count++;
		}
		//add data into stockinformationList
		ui.stockInformationList->clear();
		ui.stockInformationList->addItem(new QListWidgetItem(QString(("Surprise: " + std::to_string(s.GetSurprise()).substr(0, 7) + "%").c_str())));
		ui.stockInformationList->addItem(new QListWidgetItem(QString(("Report Date: " + s.GetRepDate().toString()).c_str())));
		ui.stockInformationList->addItem(new QListWidgetItem(QString(("Security: " + s.GetSecurity()).c_str())));
		ui.stockInformationList->addItem(new QListWidgetItem(QString(("GICS Sector: " + s.GetSector()).c_str())));
		ui.stockInformationList->addItem(new QListWidgetItem(QString(("GICS Sub Industry: " + s.GetSubIndustry()).c_str())));
		ui.stockInformationList->addItem(new QListWidgetItem(QString(("Address: " + s.GetAddress()).c_str())));
		ui.stockInformationList->addItem(new QListWidgetItem(QString(("CIK: " + s.GetCik()).c_str())));
	}
	ui.stocksTable->show();
}

/*private utility function*/
void Project6883::addGroupTableElement(Group big_group) {
	int row_max = big_group.GetSize();
	ui.groupTable->setRowCount(row_max);
	std::vector<std::string> tickers = big_group.GetMapKeys();
	for (int i = 0; i != row_max; i++) {
		//Dynamic allocation using smart pointers
		//column 0
		QTableWidgetItem *item1 = new QTableWidgetItem(tickers[i].c_str());
		item1->setFlags(item1->flags() ^ Qt::ItemIsEditable);
		ui.groupTable->setItem(i, 0, item1);
		//column 1: surpurise
		//std::cout << big_group.GetStock(tickers[i]).GetSurprise() << endl;
		QTableWidgetItem *item2 = new QTableWidgetItem((std::to_string(big_group.GetStock(tickers[i]).GetSurprise()).substr(0, 7) + "%").c_str());
		item2->setFlags(item2->flags() ^ Qt::ItemIsEditable);
		ui.groupTable->setItem(i, 1, item2);
	}
}

/*slot function for groupComboBox currentText Changed signals*/
/*The signal function is: void currentTextChanged(const QString &text)*/
void Project6883::on_groupComboBox_currentTextChanged(const QString & text) {
	//clear contents first
	ui.groupTable->clearContents();
	//then add new items
	if (text == tr("Beat")) {
		Group big_group = experiment.GetBigGroup(0);
		addGroupTableElement(big_group);
		drawGraph(0);
	} else if (text == tr("Meet")) {
		Group big_group = experiment.GetBigGroup(1);
		addGroupTableElement(big_group);
		drawGraph(1);
	} else if (text == tr("Miss")) {
		Group big_group = experiment.GetBigGroup(2);
		addGroupTableElement(big_group);
		drawGraph(2);
	} else {
		ui.groupTable->setRowCount(0);
		drawGraph(3);
	}
}

/*slot function for cell double click signal*/
void Project6883::on_groupTable_cellDoubleClicked(int row, int column) {
	if (column == 0 && ui.groupComboBox->currentText() != "") {
		QTableWidgetItem *item = new QTableWidgetItem();
		item = ui.groupTable->item(row, column);
		ui.stockEdit->setText(item->text());
		on_searchButton_clicked();
		ui.tabWidget->setCurrentIndex(0);
	}
}

/*function for ploting in GUI*/
void Project6883::drawGraph(int group) {
	//transfer Vector into QVector
	QVector<double> x(90), beatCAAR(90), meetCAAR(90), missCAAR(90);
	std::list<Vector<double, long> > CAARlist = experiment.GetCAARlist();
	Vector<double, long> beat = CAARlist.front();
	CAARlist.pop_front();
	Vector<double, long> meet = CAARlist.front();
	CAARlist.pop_front();
	Vector<double, long> miss = CAARlist.front();
	auto begin = beat.MinIndex();
	for (auto i = beat.MinIndex(); i != beat.MaxIndex() + 1; i++) {
		x[i - begin] = i - 30;
		beatCAAR[i - begin] = beat[i];
		meetCAAR[i - begin] = meet[i];
		missCAAR[i - begin] = miss[i];
	}
	//plot
	ui.plot->clearGraphs();
	ui.plot->addGraph(ui.plot->xAxis, ui.plot->yAxis);
	ui.plot->addGraph();
	ui.plot->addGraph();
	ui.plot->graph(0)->setData(x, beatCAAR);
	ui.plot->graph(0)->setPen(QPen(Qt::blue, 1.5));
	ui.plot->graph(1)->setData(x, meetCAAR);
	ui.plot->graph(1)->setPen(QPen(Qt::blue, 1.5));
	ui.plot->graph(2)->setData(x, missCAAR);
	ui.plot->graph(2)->setPen(QPen(Qt::blue, 1.5));
	//set curve's color to red
	if (group != 3) {
		ui.plot->graph(group)->setPen(QPen(Qt::red, 1.5));
	}
	//other settings
	ui.plot->xAxis->setLabel("Time");
	ui.plot->yAxis->setLabel("CAAR(Percent)");
	ui.plot->xAxis->setRange(-30, 60);
	ui.plot->yAxis->setRange(-10, 10);
	ui.plot->replot();
}

//slot function when text change
void Project6883::on_radar_change(const QString &text) {
	ui.listWidget->clear();
	if (text != NULL) {
		ui.listWidget->setHidden(false);
	}
	else {
		ui.listWidget->setHidden(true);
		ui.listWidget->clear();
		return;
	}
	//search
	std::string ticker = text.toUpper().toStdString();
	vector<std::string> result = all_tickers.search(ticker);
	//hide the listWidget when no result;
	if (result.size() == 0) {
		ui.listWidget->setHidden(true);
		ui.listWidget->clear();
		return;
	}
	for (auto i = result.begin(); i != result.end(); i++) {
		QListWidgetItem *test = new QListWidgetItem(QString(i->c_str()));
		ui.listWidget->addItem(test);
	}
}

//slot function by double click the listwidget item
void Project6883::on_listWidget_itemDoubleClicked(QListWidgetItem *item) {
	ui.stockEdit->setText(item->text());
	on_searchButton_clicked();
	ui.listWidget->setHidden(true);
}