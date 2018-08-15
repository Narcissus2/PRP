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
	//cout << "prob num_node = " <<  prob.num_node() << endl;
	x.resize(prob.num_node()+1);
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
	for(int i=0;i<prob.num_node();i++)
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
//        for(size_t customer=0; customer<prob.num_node(); customer++)
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
	/*
		隨機排序後依據車子載重和time window 去塞倉庫
	*/
	CIndividual::TDecVec &x = indv->vars();
	const vector<Node> & node = prob.node();
	const vector<vector<double>> &distance = prob.dis();
	x.resize(prob.num_node());
	for (size_t i = 0; i < x.size(); i += 1)
	{
		x[i] = i;
	}

	random_shuffle(x.begin(), x.end());
	/*cout << "shuffle ---" << endl;
	cout << "Please Enter to continue..." << endl; getchar();*/
	
	swap(x[0], *find(x.begin(), x.end(), prob.depot()));

	cout << "Permutate : " << endl;

	for (int i = 0; i < x.size(); i++)
	{
		cout << x[i] << " ";
	}
	cout << endl;
	cout << "random_permutate finish ---" << endl;
	cout << "Please Enter to continue..." << endl; getchar();

	// ----- input depot consider the capacity -----
	int capacity = 0;
	cout << "max load = " << prob.maxload() << endl;
	for (int i = 0; i < x.size(); i++)
	{
		if (x[i] == prob.depot())
		{
			capacity = 0;
		}

		if (capacity + node[x[i]].demand > prob.maxload())
		{
			std::vector<CIndividual::TGene>::iterator it;
			it = x.begin();
			x.insert(it+i, prob.depot());
			capacity = 0;
		}
		else
		{
			capacity += node[x[i]].demand;
		}
		cout << "capacity = " << capacity << endl;
	}
	x.push_back(prob.depot());
	cout << "Permutate After insert(capacity): " << endl;
	for (int i = 0; i < x.size(); i++)
	{
		cout << x[i] << " ";
	}
	cout << endl;
	cout << "insert finish ---" << endl;
	cout << "Please Enter to continue..." << endl; getchar();

	// ----- insert depot consider the time window -----
	int time = 0;
	for (int i = 0; i < x.size()-1; i++)
	{
		if (node[x[i]].ready_time + node[x[i]].service_time + distance[x[i]][x[i + 1]] / prob.avg_speed()>node[x[i + 1]].due_time)
		{
			cout << "i and i+1 : " << i << " " << i + 1 << endl;
			std::vector<CIndividual::TGene>::iterator it;
			it = x.begin();
			x.insert(it + i+1, prob.depot());
		}
	}
	cout << "Permutate After insert(time window): " << endl;
	for (int i = 0; i < x.size(); i++)
	{
		cout << x[i] << " ";
	}
	cout << endl;
	cout << "insert finish ---" << endl;
	cout << "Please Enter to continue..." << endl; getchar();

	// ----- set up the initial speed -----
	CIndividual::TObjVec & speed = indv->speed();
	speed.resize(x.size());
	for (int i = 0; i < x.size()-1; i++)
	{
		speed[i] = prob.avg_speed();
	}
	speed[x.size() - 1] = 0;
}
//---------------------------------------------------------------------------

void CRandomInitialization::operator()(CPopulation *pop, const BProblem &prob) const
{
    cout << "pop size = " << pop->size() << endl;
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
		this->random_permutate(&(*pop)[i], prob);
		//this->block_initial2(&(*pop)[i], prob); //GVRP
		//cout << "sol = " << (*pop)[i].objs()[0] << endl;
		
    }
}
