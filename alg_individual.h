#ifndef INDIVIDUAL__
#define INDIVIDUAL__

#include <vector>
#include <ostream>

// ----------------------------------------------------------------------
//		CIndividual
// ----------------------------------------------------------------------

class BProblem;

class CIndividual
{
public:
	//typedef double TGene;
	typedef int TGene; //Wu change "TGene" to int
	typedef std::vector<TGene> TDecVec;
	typedef std::vector<double> TObjVec;
	

	explicit CIndividual(std::size_t num_vars = 0, std::size_t num_objs = 0);

	TDecVec & vars() { return variables_; }
	const TDecVec & vars() const { return variables_; }

	TDecVec & routes() { return real_routes_; }
	const TDecVec & routes() const { return real_routes_; }

	TDecVec & e_up() { return e_up_; }
	const TDecVec & e_up() const { return e_up_; }

	TDecVec & e_down() { return e_down_; }
	const TDecVec & e_down() const { return e_down_; }

	TObjVec & objs() { return objectives_; }
	const TObjVec & objs() const { return objectives_; }

	TObjVec & conv_objs() { return converted_objectives_; }
	const TObjVec & conv_objs() const { return converted_objectives_; }

	TObjVec & speed() { return speeds_; }
	const TObjVec & speed() const { return speeds_; }

	void set_total_dis(double td) { total_distance_ = td; }
	const double & total_dis() const { return total_distance_; }
	
	void set_feasible(bool c) { feasible_ = c; }
	const bool feasible() const { return feasible_; }

	void set_num_vehicles(size_t nv) { num_vehicles_ = nv; }
	size_t num_vehicles() const { return num_vehicles_;  }

	size_t num_evaluate() const { return num_evaluate_; }
	void add_eva() { num_evaluate_ += 1; }
	void set_eva(size_t e) { num_evaluate_ = e; }
	void ShowRoute();

	// if a target problem is set, memory will be allocated accordingly in the constructor
	static void SetTargetProblem(const BProblem &p) { target_problem_ = &p; }
	static const BProblem & TargetProblem();

	bool operator < (const CIndividual & Indi) const
	{
		if (objectives_[0] == Indi.objectives_[0])
		{
			return objectives_[1] < Indi.objectives_[1];
		}
		return (objectives_[0] < Indi.objectives_[0]);
	}

private:

	TDecVec variables_;
	TDecVec real_routes_; // the sequence is separated by depot points
	TDecVec e_up_, e_down_;
	size_t num_vehicles_ = 0,num_evaluate_ = 0;
	double total_distance_;
	TObjVec speeds_;
	TObjVec objectives_;
	TObjVec converted_objectives_;
	bool feasible_;

	static const BProblem *target_problem_;
};

std::ostream & operator << (std::ostream &os, const CIndividual &indv);

#endif
