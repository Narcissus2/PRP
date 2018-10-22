#ifndef INITIALIZATION__
#define INITIALIZATION__


// ----------------------------------------------------------------------
//		CRandomInitialization
// ----------------------------------------------------------------------

class CIndividual;
class CPopulation;
class BProblem;

class CRandomInitialization
{
public:
	void operator()(CPopulation *pop, const BProblem &prob) const;
	void random_permutate(CIndividual *indv, const BProblem &prob) const;
	void random_permutate_nd(CIndividual *indv, const BProblem &prob) const;
	void block_initial(CIndividual *indv, const BProblem &prob) const;
	void block_initial2(CIndividual *indv, const BProblem &prob) const;
	void dp_partition(CIndividual *indv, const BProblem &prob,const int nrandom) const;
	void random_insert(CIndividual *indv, const BProblem &prob) const;
};

extern CRandomInitialization RandomInitialization;

#endif
