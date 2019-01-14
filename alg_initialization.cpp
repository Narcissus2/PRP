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
/***********************************************************
KNN()
先隨機從K個最近的點(沒倉庫)找一個點接在後面
直到全部點用完
*************************************************************/
void CRandomInitialization::KNN(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &x = indv->vars();
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & node = prob.node();
	const vector<vector<double>> &distance = prob.dis();
	x.resize(0);
	const int k = 3;
	vector<int> can_p;
	// 檢查哪些點放進route了
	vector<bool> p_not_in_route;
	for (int i = 0; i < node.size(); i++)
	{
		p_not_in_route.push_back(true);
	}
	p_not_in_route[0] = false;

	int now_p = 0; // 現在要接在誰後面
	for (int i = 0; i<node.size() - 1; i++) // 總共要放node.size()-1 個點
	{
		can_p.clear();
		//can_p.resize(k);
		//double dis_tmp = 1e9;
		for (int j = 1; j < node.size(); j++) // 每個點要檢查所有其他點
		{
			int this_p = j;
			if (p_not_in_route[this_p])
			{
				//找出最小的k個
				if (can_p.size() < k) //不滿k個直接放進去
				{
					//cout << "this p = " << this_p << endl;
					can_p.push_back(this_p);
				}
				else
				{
					// 去跟裡面的k 個比
					for (int m = 0; m < can_p.size(); m++)
					{
						if (distance[now_p][this_p] < distance[now_p][can_p[m]])
						{
							int tmp_num = can_p[m];
							//cout << "this pp = " << this_p << endl;
							can_p[m] = this_p;
							this_p = tmp_num;
						}
					}
				}
			}	
		}
		int put_in = rand() % can_p.size();
		/*for (int a = 0; a < can_p.size(); a++)
		{
			cout << "canp " << a << " = " << can_p[a] << endl;
		}
		cout << "not in route : ";
		for (int a = 0; a < p_not_in_route.size(); a++)
		{
			if (p_not_in_route[a])
			{
				cout << " " << a ;
			}
		}
		cout << endl;
		cout << "put in = " << can_p[put_in] << endl;*/
		x.push_back(can_p[put_in]);
		now_p = can_p[put_in];
		p_not_in_route[can_p[put_in]] = false;
	}
	routes = x;
	//cout << *indv << endl; getchar();

	// ----- set up the initial speed -----
	CIndividual::TObjVec & speed = indv->speed();
	speed.resize(x.size()*1.3);
	for (int i = 0; i < speed.size(); i++)
	{
		//原本speed是時速幾KM，但地圖是給M，所以要乘1000，而且時間是給秒，所以要除3600，看來還是之後才能做這些
		//speed[i] = prob.avg_speed(); // 單位是 m/s
		speed[i] = prob.want_speed(); // 90 km/h
	}
	for (int i = 0; i < speed.size(); i++) // 檢查這裡有沒有動到速度
	{
		if (speed[i] != 20.9294 && speed[i] != 15.3303)
		{
			cout << "i = " << i << endl;
			cout << "ini this speed is " << speed[i] << endl; getchar();
		}
	}

}



/***********************************************************
random permutate()
先隨機產生隨機序列(沒倉庫)
然後開始把倉庫放進去
(依照load還有 time window，time window的看法是用avg speed去算時間，超時就判斷為下一台車)
然後speed 全部設為 平均值
*************************************************************/

void CRandomInitialization::random_permutate(CIndividual *indv, const BProblem &prob) const
{
	/*
	隨機排序後依據車子載重和time window 去塞倉庫
	*/
	CIndividual::TDecVec &x = indv->vars();
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & node = prob.node();
	const vector<vector<double>> &distance = prob.dis();
	x.resize(prob.num_node() - 1);
	for (size_t i = 0; i < x.size(); i += 1)
	{
		x[i] = i + 1;
	}
	random_shuffle(x.begin(), x.end());
	/*cout << "shuffle ---" << endl;
	cout << "Please Enter to continue..." << endl; getchar();*/
	//swap(x[0], *find(x.begin(), x.end(), prob.depot()));
	// put x in routes 
	routes.resize(0);
	routes.push_back(0);
	for (int i = 0; i < x.size(); i++)
	{
		routes.push_back(x[i]);
	}
	/*cout << "Permutate : " << endl;

	for (int i = 0; i < x.size(); i++)
	{
	cout << x[i] << " ";
	}
	cout << endl;
	cout << "random_permutate finish ---" << endl;
	cout << "Please Enter to continue..." << endl; getchar();*/

	// ----- input depot consider the capacity -----
	int capacity = 0;
	//cout << "max load = " << prob.maxload() << endl;
	for (int i = 0; i < routes.size(); i++)
	{
		if (routes[i] == prob.depot())
		{
			capacity = 0;
		}

		if (capacity + node[routes[i]].demand > prob.maxload())
		{
			std::vector<CIndividual::TGene>::iterator it;
			it = routes.begin();
			routes.insert(it + i, prob.depot());
			capacity = 0;
		}
		else
		{
			capacity += node[routes[i]].demand;
		}
		//cout << "capacity = " << capacity << endl;
	}
	routes.push_back(prob.depot());
	/*cout << "Permutate After insert(capacity): " << endl;
	for (int i = 0; i < x.size(); i++)
	{
	cout << x[i] << " ";
	}
	cout << endl;
	cout << "insert finish ---" << endl;
	cout << "Please Enter to continue..." << endl; getchar();*/

	// ----- insert depot consider the time window -----
	int time = 0;
	for (int i = 0; i < routes.size() - 1; i++)
	{
		if (node[routes[i]].ready_time + node[routes[i]].service_time + distance[routes[i]][routes[i + 1]] / prob.avg_speed()>node[routes[i + 1]].due_time)
		{
			cout << "i and i+1 : " << i << " " << i + 1 << endl;
			std::vector<CIndividual::TGene>::iterator it;
			it = routes.begin();
			routes.insert(it + i + 1, prob.depot());
		}
	}
	/*cout << "Permutate After insert(time window): " << endl;
	cout << *indv << endl; // print x
	cout << "insert finish ---" << endl;
	cout << "Please Enter to continue..." << endl; getchar();*/

	// ----- set up the initial speed -----
	CIndividual::TObjVec & speed = indv->speed();
	speed.resize(routes.size() - 1);
	for (int i = 0; i < routes.size() - 1; i++)
	{
		//原本speed是時速幾KM，但地圖是給M，所以要乘1000，而且時間是給秒，所以要除3600，看來還是之後才能做這些
		//speed[i] = prob.avg_speed(); // 單位是 m/s
		speed[i] = prob.want_speed(); // 90 km/h
	}
}
//---------------------------------------------------------------------------

/***********************************************************
random permutate_nd()
隨機產生隨機序列(沒倉庫)
nd = no depot
然後speed 全部設為 平均值
*************************************************************/

void CRandomInitialization::random_permutate_nd(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &x = indv->vars();
	const vector<Node> & node = prob.node();
	CIndividual::TDecVec &routes = indv->routes();
	const vector<vector<double>> &distance = prob.dis();
	x.resize(prob.num_node() - 1); // 不需要倉庫那個點，所以就產生序列個數就好(prob.num_node()-1)
	for (size_t i = 0; i < x.size(); i += 1)
	{
		x[i] = i + 1;
	}

	random_shuffle(x.begin(), x.end());
	routes = x;
	/*cout << "shuffle ---" << endl;
	cout << "Please Enter to continue..." << endl; getchar();*/

	//swap(x[0], *find(x.begin(), x.end(), prob.depot())); // 不用depot 了

	/*cout << "Permutate : " << endl;

	for (int i = 0; i < x.size(); i++)
	{
	cout << x[i] << " ";
	}
	cout << endl;
	cout << "random_permutate finish ---" << endl;
	cout << "Please Enter to continue..." << endl; getchar();*/

	//----- set up the initial speed (好像還不用設定)-----
	CIndividual::TObjVec & speed = indv->speed();
	speed.resize(x.size()*1.3);
	for (int i = 0; i < speed.size(); i++)
	{
		//原本speed是時速幾KM，但地圖是給M，所以要乘1000，而且時間是給秒，所以要除3600，看來還是之後才能做這些
		//speed[i] = prob.avg_speed();
		speed[i] = prob.want_speed(); // 90 km/h
	}
	for (int i = 0; i < speed.size(); i++) // 檢查這裡有沒有動到速度
	{
		if (speed[i] != 20.9294 && speed[i] != 15.3303)
		{
			cout << "i = " << i << endl;
			cout << "ini2 this speed is " << speed[i] << endl; getchar();
		}
	}

}

//---------------------------------------------------------------------------

void CRandomInitialization::operator()(CPopulation *pop, const BProblem &prob) const
{
	cout << "pop size = " << pop->size() << endl;
	//const size_t n_random_num = 0;
	for (size_t i = 0; i<pop->size(); i += 1)
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
		if (i < 50)
		{
			this->KNN(&(*pop)[i], prob);
		}
		else
		{
			this->random_permutate_nd(&(*pop)[i], prob);
		}
		

		//cout << "sol = " << (*pop)[i].objs()[0] << endl;
	}
}
