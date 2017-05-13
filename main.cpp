#include "Project6883.h"
#include "ExcelDriver\ExcelDriver.hpp"
#include "ExcelDriver\BitsAndPieces\StringConversions.hpp"
#include "ExcelDriver\ExcelMechanisms.hpp"
#include "ExcelDriver\VectorsAndMatrices\NumericMatrix.cpp" // Template
#include "ExcelDriver\ExceptionClasses\DatasimException.hpp"
#include <QtWidgets/QApplication>
#include <QtCore>
#include <stdio.h>
#include "curl.h"
#include "date.h"
#include "equity.h"
#include "datahandle.h"
#include <map>
#include <stack>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	Project6883 w;
	w.show();
	return a.exec();
}
