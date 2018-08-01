
#include "problem_factory.h"
#include "problem_self.h"
#include <string>
using namespace std;

BProblem *GenerateProblem(std::ifstream &ifile)
{
	//read the .ini in Experiments file
	string pname, dummy;
	ifile >> dummy >> dummy >> pname; // problem.name(dummy) =(dummy) [ ](pname) //NAME : E016-03m
	
	size_t num_vars, num_objs;
	ifile >> dummy >> dummy >> num_vars;
	ifile >> dummy >> dummy >> num_objs;
	return new CProblemSelf(num_vars, num_objs,pname);

	return 0;
}
