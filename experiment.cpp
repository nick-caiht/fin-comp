#include "experiment.h"

Experiment::Experiment() {
}

Experiment::Experiment(double t_h, int r_t) :threshold(t_h), resample_times(r_t) {
}

Experiment::~Experiment() {
}

const NumericMatrix<double, long>& Experiment::GetAARavg() const {
	return AAR_avg;
}

const NumericMatrix<double, long>& Experiment::GetCAARavg() const {
	return CAAR_avg;
}

const double Experiment::GetThreshold() const {
	return threshold;
}

const int Experiment::GetResampleTimes() const {
	return resample_times;
}

bool Experiment::SetThreshold(double t_h) {
	if (t_h <= 10 && t_h >= 5) {
		threshold = t_h;
		return true;
	}
	else {
		return false;
	}
}

bool Experiment::SetResampleTimes(int r_t) {
	resample_times = r_t;
	return true;
}

Group Experiment::Sampling(const Group& b_g) {
	Group subgroup;
	srand(time(NULL));
	std::vector<std::string> keys = b_g.GetMapKeys();
	for (int i = 0; i < 40; ++i) {
		subgroup.PushStock(b_g.GetStock(keys[std::rand() % keys.size()]));
	}
	return subgroup;
}

bool Experiment::UpdateGroups(const std::map<std::string, Stock>& allstocks_) {
	if (allstocks_.empty()) {
		return false;
	}
	for (auto i = allstocks_.begin(); i != allstocks_.end(); i++) {
		if (i->second.GetSurprise() > threshold) {
			//Beat
			big_groups[0].PushStock(i->second);
		}
		else if (i->second.GetSurprise() < -threshold) {
			//Miss
			big_groups[2].PushStock(i->second);
		}
		else {
			//Meet
			big_groups[1].PushStock(i->second);
		}
	}
	return true;
}

bool Experiment::DoExperiment() {
	for (int i = 0; i < resample_times; ++i) {
		for (int j = 0; j < 3; ++j) {
			Group SubGroup = Sampling(big_groups[j]);
			SubGroup.Compute();
			std::vector<double> AAR = SubGroup.GetAAR();
			std::vector<double> CAAR = SubGroup.GetCAAR();
			for (int k = 0; k < 91; ++k) {
				AAR_avg(j, k) = (AAR_avg(j, k) * i + AAR[k]) / (i + 1);
				CAAR_avg(j, k) = (AAR_avg(j, k) * i + CAAR[k]) / (i + 1);
			}
		}
	}
}
