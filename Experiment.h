#pragma once

#include<vector>

#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include "ExcelDriver\VectorsAndMatrices\NumericMatrix.hpp"
#include "Group.h"

class Experiment {
private:
	std::vector<Group> big_groups = std::vector<Group>(3); //0:beat 1:meet 2:miss
	double threshold = 5;                // 5~10
	int resample_times = 1;            // 1~100
	NumericMatrix<double, long> AAR_avg = NumericMatrix<double, long>(3, 91);
	NumericMatrix<double, long> CAAR_avg = NumericMatrix<double, long>(3, 91);
public:
	Experiment();
	Experiment(double t_h, int r_t);
	~Experiment();

	const NumericMatrix<double, long>& GetAARavg() const;
	const NumericMatrix<double, long>& GetCAARavg() const;
	const double GetThreshold() const;
	const int GetResampleTimes() const;
	bool SetThreshold(double t_h);
	bool SetResampleTimes(int r_t);
	Group Sampling(const Group& b_g);     //Random 40
	bool UpdateGroups(const std::map<std::string, Stock>& allstocks_);
	bool DoExperiment();
};
