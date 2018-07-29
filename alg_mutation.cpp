
#include "alg_mutation.h"
#include "alg_individual.h"
#include "aux_math.h"
#include "problem_base.h"

#include <cstddef>
#include <algorithm>
using std::size_t;

// ----------------------------------------------------------------------
// The implementation was adapted from the code of function real_mutate_ind() in mutation.c in
// http://www.iitk.ac.in/kangal/codes/nsga2/nsga2-gnuplot-v1.1.6.tar.gz
//
// ref: http://www.slideshare.net/paskorn/simulated-binary-crossover-presentation#
// ---------------------------------------------------------------------
//下面我用不到 先mark掉-------------------------------------------------------------------
//bool CPolynomialMutation::operator()(CIndividual *indv, double mr, double eta) const
//{
//    //int j;
//    //double rnd, delta1, delta2, mut_pow, deltaq;
//    //double y, yl, yu, val, xy;
//
//	bool mutated = false;
//
//	CIndividual::TDecVec &x = indv->vars();
//
//    for (size_t i=0; i<x.size(); i+=1)
//    {
//		if (MathAux::random(0.0, 1.0) <= mr)
//        {
//			mutated = true;
//
//            double y = x[i],
//			       lb = CIndividual::TargetProblem().lower_bounds()[i],
//			       ub = CIndividual::TargetProblem().upper_bounds()[i];
//
//            double delta1 = (y-lb)/(ub-lb),
//                   delta2 = (ub-y)/(ub-lb);
//
//			double mut_pow = 1.0/(eta+1.0);
//
//			double rnd = MathAux::random(0.0, 1.0), deltaq = 0.0;
//            if (rnd <= 0.5)
//            {
//                double xy = 1.0-delta1;
//                double val = 2.0*rnd+(1.0-2.0*rnd)*(pow(xy,(eta+1.0)));
//                deltaq =  pow(val,mut_pow) - 1.0;
//            }
//            else
//            {
//                double xy = 1.0-delta2;
//                double val = 2.0*(1.0-rnd)+2.0*(rnd-0.5)*(pow(xy,(eta+1.0)));
//                deltaq = 1.0 - (pow(val,mut_pow));
//            }
//
//            y = y + deltaq*(ub-lb);
//			y = std::min(ub, std::max(lb, y));
//
//            x[i] = y;
//        }
//    }
//
//	return mutated;
//}// CPolynomialMutation
//----------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------
//		CRandomTwoSwapMutation
// ----------------------------------------------------------------------------------
bool CRandomTwoSwapMutation::operator()(CIndividual *indv, double mr) const
{
	bool mutated = false;

	CIndividual::TDecVec &x = indv->vars();
	for (size_t i = 1; i<x.size(); i += 1)
	{
		if (MathAux::random(0.0, 1.0) <= mr)
		{
			mutated = true;
			size_t newpos = rand() % (x.size()-1)+1;
			while (newpos == i) { newpos = rand() % (x.size() - 1) + 1; }

			swap(x[i], x[newpos]);
		}
	}
	return mutated;
}// CRandomTwoSwapMutation


// ----------------------------------------------------------------------------------
//		CRandomTwoSwapMutationNoDepot
// ----------------------------------------------------------------------------------
bool CRandomTwoSwapMutationNoDepot::operator()(CIndividual *indv, double mr) const
{
	bool mutated = false;

	CIndividual::TDecVec &x = indv->vars();
    CIndividual::TGene depot = x[0];
    for (size_t i=0; i<x.size(); i+=1)
    {
		if (x[i] != depot && MathAux::random(0.0, 1.0) <= mr)
        {
			mutated = true;
            size_t  newpos;

            while(true)
            {
                newpos = rand() % x.size();
                if(x[newpos] != depot) break;
            }
            swap(x[i],x[newpos]);
        }
    }
	return mutated;
}// CRandomTwoSwapMutationNoDepot


 // ----------------------------------------------------------------------------------
 //		CRandomTwoPointMutation
 // ----------------------------------------------------------------------------------
bool CRandomTwoPointMutation::operator()(CIndividual *indv, double mr) const
{
	bool mutated = false;
	
	CIndividual::TDecVec &x = indv->routes();
	CIndividual::TGene depot = x[0];
	//if (x.size() <= indv->num_vehicles()) return mutated;
	
	//mutation according mr
	if (MathAux::random(0.0, 1.0) <= mr)
	{
		mutated = true;
		/*cout << "x = ";
		for (size_t i = 0; i < x.size(); i++)
		{
			cout << x[i] << " ";
		}
		cout << endl;*/
		int rand_cus[4] = {0}; //0<->1 , 2<->3
		//find first change point
		//cout << x[0] << endl;
		//找出第一個準備交換的部分
		while (x[rand_cus[0]] == depot)
		{
			rand_cus[0] = rand() % x.size();
			//在邊界0 or x.size()就不能做後面位移判斷(後面會超出邊界)，這是判斷這個區間至少有兩個客戶
			if ((rand_cus[0] != x.size() - 1 && rand_cus[0] != 0) && (x[rand_cus[0] + 1] == depot && x[rand_cus[0] - 1] == depot))
			{
				rand_cus[0] = 0;
			}
		}

		size_t left[2] = { 0 }, right[2] = { 0 }; //左右邊倉庫的index
		left[0] = rand_cus[0];
		//找出第一部分的左邊界index(倉庫)
		while (x[left[0]] != depot)
		{
			left[0]--;
		}
		right[0] = rand_cus[0];
		//找出第一部分的右邊界index(倉庫)
		while (x[right[0]] != depot)
		{
			right[0]++;
		}
		//找出第二要準備交換的部分
		while (x[rand_cus[1]] == depot)
		{
			rand_cus[1] = rand() % x.size();
			if ((rand_cus[1] != x.size() - 1 && rand_cus[1] != 0) && (x[rand_cus[1] + 1] == depot && x[rand_cus[1] - 1] == depot))
			{
				rand_cus[1] = 0;
			}
		}
		left[1] = rand_cus[1];
		while (x[left[1]] != depot)
		{
			left[1]--;
		}
		
		right[1] = rand_cus[1];
		while (x[right[1]] != depot)
		{
			right[1]++;
		}

		rand_cus[2] = rand_cus[0] + 1;
		rand_cus[3] = rand_cus[1] + 1;
		if (x[rand_cus[2]] == depot) {
			rand_cus[0] -= 1;
			rand_cus[2] -= 1;
		}
		if (x[rand_cus[3]] == depot) {
			rand_cus[1] -= 1;
			rand_cus[3] -= 1;
		}
		/*cout << "x.size = " << x.size() << endl;
		cout << x[rand_cus[0]] << "[" << rand_cus[0] << "] change " << x[rand_cus[1]] << "[" << rand_cus[1] << "]" << endl;
		cout << x[rand_cus[2]] << "[" << rand_cus[2] << "] change " << x[rand_cus[3]] << "[" << rand_cus[3] << "]" << endl;*/
		swap(x[rand_cus[0]], x[rand_cus[1]]);
		//swap(x[rand_cus[2]], x[rand_cus[3]]);//這個mark掉就是one point
		//system("pause");
	}
	

	
	return mutated;
}// CRandomTwoPointMutation