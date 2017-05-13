#include "experiment.h"

Experiment::Experiment() {
	for (long r = AAR_CAAR.MinRowIndex(); r <= AAR_CAAR.MaxRowIndex(); ++r) {
		for (long col = AAR_CAAR.MinColumnIndex(); col <= AAR_CAAR.MaxColumnIndex(); ++col) {
			AAR_CAAR(r, col) = Vector<double, long>(90);
		}
	}
}

Experiment::Experiment(double t_h, int r_t) : threshold(t_h), resample_times(r_t) {
	for (long r = AAR_CAAR.MinRowIndex(); r <= AAR_CAAR.MaxRowIndex(); ++r) {
		for (long col = AAR_CAAR.MinColumnIndex(); col <= AAR_CAAR.MaxColumnIndex(); ++col) {
			AAR_CAAR(r, col) = Vector<double, long>(90);
		}
	}
}

Experiment::Experiment(const Experiment & exp) : big_groups(exp.big_groups), threshold(exp.threshold), resample_times(exp.resample_times), AAR_CAAR(exp.AAR_CAAR) {
}

Experiment::~Experiment() {
}

std::list< Vector<double, long> > Experiment::GetAARlist() const {
	std::list<Vector<double, long> > aar_list;
	for (int r = AAR_CAAR.MinRowIndex(); r <= AAR_CAAR.MaxRowIndex(); ++r) {
		aar_list.push_back(AAR_CAAR(r, 1));
	}
	return aar_list;
}

std::list< Vector<double, long> > Experiment::GetCAARlist() const {
	std::list<Vector<double, long> > caar_list;
	for (int r = AAR_CAAR.MinRowIndex(); r <= AAR_CAAR.MaxRowIndex(); ++r) {
		caar_list.push_back(AAR_CAAR(r, 2));
	}
	return caar_list;
}

const double Experiment::GetThreshold() const {
	return threshold;
}

const int Experiment::GetResampleTimes() const {
	return resample_times;
}

const Group & Experiment::GetBigGroup(int index) const {
	return big_groups[index];
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

/* From the big group, randomly select 40 stocks with replacement, and return a group of 40*/
Group Experiment::Sampling(const Group& b_g) {
	Group subgroup;
	srand(time(NULL));
	std::vector<std::string> keys = b_g.GetMapKeys();
	for (int i = 0; i < 40; ++i) {
		//randomly select a ticker from MapKeys,
		//use this ticker to get stock from big group and push into subgroup
		subgroup.PushStock(b_g.GetStock(keys[std::rand() % keys.size()]));
	}
	return subgroup;
}

/* Distribute the stocks in stock pool into big groups by threshold */
bool Experiment::UpdateGroups(const std::map<std::string, Stock>& allstocks_) {
	if (allstocks_.empty()) {
		return false;
	}
	for (auto i = allstocks_.begin(); i != allstocks_.end(); i++) {
		if ((i->second).GetSurprise() > threshold) {
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

/* Bootsraping n times and calculate the average AAR & CAAR */
bool Experiment::DoExperiment(const ETF& SPY_) {
	for (int i = 0; i < resample_times; ++i) {
		//for each big group
		for (int j = 1; j <= 3; ++j) {
			Group SubGroup = Sampling(big_groups[j - 1]);
			SubGroup.Compute(SPY_);
			std::vector<double> AAR = SubGroup.GetAAR();
			std::vector<double> CAAR = SubGroup.GetCAAR();

			for (int k = 1; k <= 90; ++k) {
				//calculate running average of AAR
				AAR_CAAR(j, 1)[k] = (AAR_CAAR(j, 1)[k] * i + AAR[k - 1]) / (i + 1);
				//calculate running average of CAAR
				AAR_CAAR(j, 2)[k] = (AAR_CAAR(j, 2)[k] * i + CAAR[k - 1]) / (i + 1);

			}
		}
	}
	return true;
}