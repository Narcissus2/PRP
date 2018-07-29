#ifndef BASE_PROBLEM__
#define BASE_PROBLEM__

#include <string>
#include <vector>
#include <iostream>

using namespace std;

// ----------------------------------------------------------------------
//		BProblem: the base class of problems (e.g. ZDT and DTLZ)
// ----------------------------------------------------------------------
class CIndividual;
struct Node
    {
        double x,y,demand;
    };

class BProblem
{
public:
	explicit BProblem(const std::string &name):name_(name) {}
	virtual ~BProblem() {}

	virtual std::size_t num_variables() const = 0;
	virtual std::size_t num_objectives() const = 0;
	virtual bool EvaluateDpCar(CIndividual *indv) const = 0;
	virtual bool Dp2Object(CIndividual *indv,int obj1_rate) const = 0;
	virtual bool EvaluateOldEncoding(CIndividual *indv) const = 0;
	

	const std::vector<Node> & node() const {return node_;}
	const std::vector<vector<double>> & dis() const { return distance_; }
	const std::vector<vector<int>> & dimen()const { return dimen_; }
	const std::vector<int> & ori_route() const { return ori_route_; }
	void PrintNode() const {cout << node_.size();}

	const std::string & name() const { return name_; }
	const std::vector<double> & lower_bounds() const { return lbs_; }
	const std::vector<double> & upper_bounds() const { return ubs_; }
	const std::size_t depot()const {return depot_section_;}
	const std::size_t capacity() const {return capacity_;}
	const std::size_t num_vehicles() const {return num_vehicles_;}
	const std::size_t dimension() const {return dimension_;}
	const std::size_t num_dimen() const { return num_dimen_; }
	
    //std::ostream & operator << (std::ostream &os, const Node &node);
	//ostream & operator <<(ostream & os ,const Node& node)
	/*std::ostream & operator << (std::ostream &os, const Node &node)
	{
	    std::cout <<" x = " << node[0].x << endl;
	    std::cout << "y = " << node[0].y << endl;
	    std::cout << "demand = " << node[0].demand << endl;
	    return os;
	}*/

protected:
	std::string name_;
    std::vector<Node> node_;
	std::vector<vector<double>> distance_;
	std::vector<vector<int>>dimen_;
	std::vector<int> ori_route_;
	std::size_t depot_section_, capacity_, dimension_ ,num_vehicles_,num_dimen_;
	//int num_vehicles_;
	double feasible_dis_;
	std::vector<double> lbs_, // lower bounds of variables
		                ubs_; // upper bounds of variables
};

#endif
