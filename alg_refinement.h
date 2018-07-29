#pragma once

#ifndef REFINEMENT__
#define REFINEMENT__

// ----------------------------------------------------------------------------------
//		CNearestNeighborRefinement : nearest neighbor refinement
// ----------------------------------------------------------------------------------

class CIndividual;
class BProblem;

class CNearestNeighborRefinement
{
public:
	bool operator()(CIndividual *c, const BProblem &prob) const;
};

class CRandomNearestNeighborRefinement
{
public:
	bool operator()(CIndividual *c, const BProblem &prob) const;
};

class CPartialNearestNeighborRefinement
{
public:
	bool operator()(CIndividual *c, const BProblem &prob) const;
};

class CFartoNearSort
{
public:
	bool operator()(CIndividual *c, const BProblem &prob) const;
};

class CReDispatchRefinement
{
public:
	bool operator()(CIndividual *c, const BProblem &prob) const;
};

class CNEHRefinement
{
public:
	bool operator()(CIndividual *c, const BProblem &prob) const;
};

class CRandomNEHRefinement
{
public:
	bool operator()(CIndividual *c, const BProblem &prob) const;
};

class CPartialNEHRefinement
{
public:
	bool operator()(CIndividual *c, const BProblem &prob) const;
};

class CPartNEH2Object
{
public:
	bool operator()(CIndividual *c, const BProblem &prob,const int obj1_rate, const double normal_dis, const double normal_emi) const;
};
class CPartNN2Object
{
public:
	bool operator()(CIndividual *c, const BProblem &prob, const int obj1_rate, const double normal_dis, const double normal_emi) const;
};

class CTwoOpt2Object
{
public:
	bool operator()(CIndividual *c, const BProblem &prob, const int obj1_rate, const double normal_dis, const double normal_emi, const double Min_dis, const double Min_emi) const;
};
class COnePointOpt
{
public:
	bool operator()(CIndividual *c, const BProblem &prob) const;
};
class CAllPointOpt
{
public:
	bool operator()(CIndividual *c, const BProblem &prob, const int obj1_rate, const double normal_dis, const double normal_emi, const double Min_dis, const double Min_emi) const;
};

class COnePointOptGlobal
{
public:
	bool operator()(CIndividual *c, const BProblem &prob) const;
};

class CTwoOpt
{
public:
	bool operator()(CIndividual *c, const BProblem &prob) const;
};

class COnePointRelocation
{
public:
	bool operator()(CIndividual *c, const BProblem &prob) const;
};

class CReArrangePermutation
{
public:
	bool operator()(CIndividual *c, const BProblem &prob) const;
};

#endif