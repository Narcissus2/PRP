#include "alg_initialization.h"
#include "problem_base.h"
#include "alg_individual.h"
#include "alg_population.h"
#include "aux_math.h"
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include "alg_refinement.h"
//Wu*
#include <iostream>
#include "ctimer.h"
using namespace std;
//*Wu
#include <cstddef>
using std::size_t;

CRandomInitialization RandomInitialization;

//Random version initial
/**void CRandomInitialization::operator()(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &x = indv->vars();
	const vector<Node> & n= prob.node();
	//x.resize(prob.num_variables());
	//cout << "prob dimension = " <<  prob.dimension() << endl;
	x.resize(prob.dimension()+1);
	int load = 0;
    //cout << "depot = " << prob.depot() << endl;
    x[0] = prob.depot();
	for (size_t i=1; i<x.size()-1; i+=1)
	{
		x[i] = ;
		bool same = false;
		do
        {
            same = false;
            for(int j=0;j<i;j++)
            {
                if(x[i] == x[j])
                {
                    x[i] = rand()%prob.num_vehicles();
                    same = true;
                    break;
                }
            }

        }while(same);
        //cout << "??init x[" << i << "] = " << x[i]-1 << endl;
        load+= prob.node()[x[i]-1].demand;

        //cout << "init x[i] = " << x[i] << endl;
        //cout << "load + " << prob.node()[x[i]-1].demand << " = " << load << endl;
        std::vector<CIndividual::TGene>::iterator it;
        it = x.begin();
        if(load > prob.capacity())
        {
            //cout << "inser -- ";
            x.insert(it+i,prob.depot());
            //cout << "insert finish\n";
            load = 0;
            i+=1;
        }
	}
	x[x.size()-1] = prob.depot(); //set depot tail

	/*cout << "x demand = ";
	for(int i=0;i<prob.dimension();i++)
    {
        cout << prob.node()[i].demand << " ";
    }
    cout << endl;

    cout << "x chromesome = ";
    for(int i=0;i<x.size();i++)
    {
        cout << x[i] << " " ;
    }
    cout << endl;*/
//cout << "load = " << load << endl;

//} //random version 1
// ----------------------------------------------------------------------


/*** version 3  random insert  **/
//目前不用這個------------------------------------------------------------------
//void CRandomInitialization::random_insert(CIndividual *indv, const BProblem &prob) const
//{
//    CIndividual::TDecVec &x = indv->vars();
//    const vector<Node> & n= prob.node();
//
//    //insert num of vehicle
//    vector <bool> full;
//    vector <double> car_load;
//
//    bool rout_is_OK = true;
//    int round = 0;
//
//    while(rout_is_OK)
//    {
//        round ++ ;
//        x.resize(prob.num_vehicles()+1);
//        for(size_t i=0; i<prob.num_vehicles()+1; i++)
//        {
//            x[i] = prob.depot();
//        }
//
//        full.resize(prob.num_vehicles());
//        car_load.resize(prob.num_vehicles());
//        for(size_t i=0; i<prob.num_vehicles(); i++)
//        {
//            full[i] = false;
//            car_load[i] = 0.0;
//        }
//
//
//        vector <bool> noload;
//        noload.resize(prob.num_vehicles());
//
//
//        for(size_t customer=0; customer<prob.dimension(); customer++)
//        {
//            //cout << "customer = " << customer << endl;
//            if(customer==prob.depot()-1)
//                continue;
//            int rand_distribution = rand()%prob.num_vehicles();
//
//            while(full[rand_distribution])
//            {
//                rand_distribution = rand() % prob.num_vehicles();
//            }
//
//            for(size_t i=0; i<prob.num_vehicles(); i++)
//            {
//                noload[i] = false;
//            }
//
//            while(car_load[rand_distribution]+n[customer].demand > prob.capacity() && rout_is_OK)
//            {
//                noload[rand_distribution] = true;
//                bool OK = false;
//                for(size_t i=0; i<prob.num_vehicles(); i++)
//                {
//                    if(!noload[i])
//                    {
//                        OK = true;
//                        break;
//                    }
//                }
//
//                if(!OK)
//                {
//                    rout_is_OK = false;
//                    break;
//                }
//
//                rand_distribution = rand() % prob.num_vehicles();
//            }
//            if(!rout_is_OK)
//                break;
//            car_load[rand_distribution]+= n[customer].demand;
// 
//            if(car_load[rand_distribution]==prob.capacity())
//                full[rand_distribution] = true;
//
//            size_t cnt_depot = 0,insert_index = 1;
//            for(; cnt_depot < rand_distribution; insert_index++)
//            {
//                if(x[insert_index]==prob.depot())
//                {
//                    cnt_depot ++;
//                }
//            }
//            std::vector<CIndividual::TGene>::iterator it;
//            it = x.begin();
//            x.insert(it+insert_index,customer+1);
//        }
//        if(rout_is_OK)
//        {
//			//Local Optimal 
//			size_t head = 1, tail = 1;
//			for (size_t i = 1; i < x.size(); i++)
//			{
//				if (x[i] == prob.depot())
//				{
//					tail = i;
//					for (size_t j = head; j < tail; j++)
//					{
//						size_t target = j;
//						double local_dis = sqrt(pow(n[x[j - 1] - 1].x - n[x[j] - 1].x, 2) + pow(n[x[j - 1] - 1].y - n[x[j] - 1].y, 2));
//						for (size_t k = j + 1; k < tail; k++)
//						{
//							double temp_dis = sqrt(pow(n[x[j - 1] - 1].x - n[x[k] - 1].x, 2) + pow(n[x[j - 1] - 1].y - n[x[k] - 1].y, 2));
//							if (temp_dis < local_dis)
//							{
//								local_dis = temp_dis;
//								target = k;
//							}
//						}
//						swap(x[j], x[target]);
//					}
//					head = tail + 1;
//				}
//
//
//			}
//            break;
//        }
//        rout_is_OK = true;
//    }
//}
//---------------------------------------------------------------------------

void CRandomInitialization::random_permutate(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &x = indv->vars();
	x.resize(prob.dimension());

	for (size_t i = 0; i < x.size(); i += 1)
	{
		x[i] = i + 1;
	}

	random_shuffle(x.begin(), x.end());

	swap(x[0], *find(x.begin(), x.end(), prob.depot()));
}
//---------------------------------------------------------------------------
void CRandomInitialization::block_initial(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &x = indv->vars();
	const vector<Node> & n = prob.node();

	x.resize(prob.dimension());
	x[0] = prob.depot();

	//把點分成4象限-------------------------------------------------------------------
	vector<int> dimen[4];
	for (size_t i = 0; i < x.size(); i += 1)
	{
		if (i + 1 == prob.depot()) continue;
		if (n[i].x-n[prob.depot()].x >= 0 && n[i].y- n[prob.depot()].y > 0)
		{
			dimen[0].push_back(i + 1);
		}
		else if (n[i].x- n[prob.depot()].x < 0 && n[i].y - n[prob.depot()].y >= 0)
		{
			dimen[1].push_back(i + 1);
		}
		else if (n[i].x- n[prob.depot()].x <= 0 && n[i].y - n[prob.depot()].y < 0)
		{
			dimen[2].push_back(i + 1);
		}
		else
		{
			dimen[3].push_back(i + 1);
		}
	}
	//-------------------------------------------------------------------
	//把各象限集合--------------------------------------------------------
	size_t x_num = 1;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < dimen[i].size();j++)
		{
			x[x_num] = dimen[i][j];
			x_num++;
		}
	}
	//-------------------------------------------------------------------
	//各象限shuffle-------------------------------------------------------------------
	for (size_t i = 0; i < 4; i++)
	{
		if (i == 0) random_shuffle(x.begin()+1, x.begin()+ 1 +dimen[0].size());
		else random_shuffle(x.begin() + dimen[i - 1].size()+1, x.begin() + dimen[i - 1].size() + dimen[i].size()+1);
	}
	//-------------------------------------------------------------------
	//random_shuffle(x.begin()+1, x.end());

	//swap(x[0], *find(x.begin(), x.end(), prob.depot()));
}

//---------------------------------------------------------------------------------

void CRandomInitialization::block_initial2(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &x = indv->vars();
	const vector<Node> & n = prob.node();
	const vector <vector<int>> & dimen = prob.dimen();
	x = prob.ori_route();

	//x.resize(prob.dimension());
	//x[0] = prob.depot();

	////把點分8象限--------------------------------------------------------------------------
	//vector<int> dimen[8];
	//const size_t dimen_num = 8;
	//for (size_t i = 0; i < x.size(); i += 1)
	//{
	//	if (i + 1 == prob.depot()) continue;
	//	if (n[i].x - n[prob.depot()-1].x >= 0 && n[i].y - n[prob.depot()-1].y > 0)
	//	{
	//		if((n[i].x - n[prob.depot()-1].x)!=0 && ((n[i].y - n[prob.depot()-1].y)/ (n[i].x - n[prob.depot()-1].x))<=1)
	//			dimen[0].push_back(i + 1);
	//		else 
	//			dimen[1].push_back(i + 1);
	//	}
	//	else if (n[i].x - n[prob.depot()-1].x < 0 && n[i].y - n[prob.depot()-1].y >= 0)
	//	{
	//		if (((n[i].y - n[prob.depot()-1].y) / (n[i].x - n[prob.depot()-1].x)) <= -1)
	//			dimen[2].push_back(i + 1);
	//		else
	//			dimen[3].push_back(i + 1);
	//	}
	//	else if ((n[i].x - n[prob.depot()-1].x) <= 0 && (n[i].y - n[prob.depot()-1].y) < 0)
	//	{
	//		if ((n[i].x - n[prob.depot()-1].x) != 0 && ((n[i].y - n[prob.depot()-1].y) / (n[i].x - n[prob.depot()-1].x)) <= 1)
	//			dimen[4].push_back(i + 1);
	//		else
	//			dimen[5].push_back(i + 1);
	//	}
	//	else
	//	{
	//		if (((n[i].y - n[prob.depot()-1].y) / (n[i].x - n[prob.depot()-1].x)) <= -1)
	//			dimen[6].push_back(i + 1);
	//		else
	//			dimen[7].push_back(i + 1);
	//	}
	//}
	////---------------------------------------------------------------------
	////分完的8象限再結合起來-------------------------------------------------------
	//size_t x_num = 1;
	//for (size_t i = 0; i < dimen_num; i++)
	//{
	//	for (size_t j = 0; j < dimen[i].size(); j++)
	//	{
	//		x[x_num] = dimen[i][j];
	//		x_num++;
	//	}
	//}
	//---------------------------------------------------------------------

	//把各象限shuffle-------------------------------------------------------------
	//MyTimers.GetTimer("shuffle")->start();
	size_t shuffle_size = 0;
	for (size_t i = 0; i < prob.num_dimen(); i++)
	{
		if (i == 0) random_shuffle(x.begin() + 1, x.begin() + dimen[0].size() + 1);
		else random_shuffle(x.begin() + shuffle_size + 1, x.begin() + shuffle_size + dimen[i].size() + 1);
		shuffle_size += dimen[i].size();
	}
	//MyTimers.GetTimer("shuffle")->end();
}

//---------------------------------------------------------------------------------

void CRandomInitialization::operator()(CPopulation *pop, const BProblem &prob) const
{
    //cout << "pop size = " << pop->size() << endl;
    const size_t n_random_num = 0;
    for (size_t i=0; i<pop->size(); i+=1)
    {
		//cout << "pop " << i << endl;
        //if(i<n_random_num)
        //{
        //    (*this)( &(*pop)[i], prob,n_random_num);
        //}
        //else (*this)( &(*pop)[i], prob );
		//this->random_permutate(&(*pop)[i], prob);
		//this->block_initial(&(*pop)[i], prob);

		//initial until all solutions feasible and different ----------------------
		//bool check = true;
		/*while (check)
		{
			check = false;
			this->block_initial2(&(*pop)[i], prob);
			for (int j = 0; j < i; j++)
			{
				if ((*pop)[j].objs()[0] == (*pop)[i].objs()[0])
				{
					check = true;
					break;
				}
			}
		}*/
		//-------------------------------------------------------

		this->block_initial2(&(*pop)[i], prob);
		//cout << "sol = " << (*pop)[i].objs()[0] << endl;
		
    }
}
