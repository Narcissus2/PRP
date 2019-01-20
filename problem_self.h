#ifndef PROBLEM_SELF__
#define PROBLEM_SELF__

#include "problem_base.h"
#include <vector>

using namespace std;
// You can add your own problem here.

class CProblemSelf : public BProblem
{

public:
	CProblemSelf(std::size_t num_vars, std::size_t num_objs, const std::string name);

	virtual std::size_t num_variables() const { return num_vars_; }
	virtual std::size_t num_objectives() const { return num_objs_; }
	virtual bool PRPDP(CIndividual *indv, double obj1_rate) const;
	virtual double Calculate_distance(const CIndividual::TDecVec &routes, int s, int e) const;
	virtual double Calculate_time( CIndividual *indv, int s, int e, int depot_num) const;
	virtual double Calculate_fuel(const CIndividual *indv, int s, int e, int depot_num) const;
	virtual bool EvaluateOldEncoding(CIndividual *indv) const;


private:
	std::size_t num_vars_, num_objs_;
	const double a_ = 6.208*0.001, b_ = 0.2125, m_ = 2.61,FCR_full_ = 0.390*10,FCR_empty_ = 0.296*1; //m = CER = CO2 rate
	//PRP
	const double time_punishment_ = 1e8, fuel_punishment_ = 1e8, distance_punishment_ = 1e8;

};

#endif
