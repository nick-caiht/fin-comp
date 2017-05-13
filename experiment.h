#pragma once

#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include "ExcelDriver\BitsAndPieces\StringConversions.hpp"
#include "ExcelDriver\ExcelDriver.hpp"
#include "ExcelDriver\ExcelMechanisms.hpp"
#include "ExcelDriver\VectorsAndMatrices\Matrix.hpp"
#include "ExcelDriver\VectorsAndMatrices\Vector.hpp"
#include "ExcelDriver\VectorsAndMatrices\NumericMatrix.cpp"
#include "ExcelDriver\ExceptionClasses\DatasimException.hpp"
#include "group.h"
#include "date.h"
#include "equity.h"

class Experiment {
private:
	std::vector<Group> big_groups = std::vector<Group>(3); //0:beat 1:meet 2:miss
	double threshold = 10;                // 5~10
	int resample_times = 1;            // 1~100
	Matrix<Vector<double, long>, long> AAR_CAAR = Matrix<Vector<double, long>, long>(3, 2);
public:
	Experiment();
	Experiment(double t_h, int r_t);
	Experiment(const Experiment& exp);
	~Experiment();

	std::list< Vector<double, long> > GetAARlist() const;
	std::list< Vector<double, long> > GetCAARlist() const;
	const double GetThreshold() const;
	const int GetResampleTimes() const;
	const Group& GetBigGroup(int index) const;
	bool SetThreshold(double t_h);
	bool SetResampleTimes(int r_t);
	Group Sampling(const Group& b_g);     //Random 40
	bool UpdateGroups(const std::map<std::string, Stock>& allstocks_);
	bool DoExperiment(const ETF& SPY_);
};
