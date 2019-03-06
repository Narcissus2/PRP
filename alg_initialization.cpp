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


/***********************************************************
KNN()
���H���qK�ӳ̪��I(�S�ܮw)��@���I���b�᭱
��������I�Χ�
*************************************************************/
void CRandomInitialization::KNN(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &x = indv->vars();
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & node = prob.node();
	const vector<vector<double>> &distance = prob.dis();
	x.resize(0);
	const int k = 2;
	vector<int> can_p;
	// �ˬd�����I��iroute�F
	vector<bool> p_not_in_route;
	for (int i = 0; i < node.size(); i++)
	{
		p_not_in_route.push_back(true);
	}
	p_not_in_route[0] = false;

	int now_p = 0; // �{�b�n���b�֫᭱
	for (int i = 0; i<node.size() - 1; i++) // �`�@�n��node.size()-1 ���I
	{
		can_p.clear();
		//can_p.resize(k);
		//double dis_tmp = 1e9;
		for (int j = 1; j < node.size(); j++) // �C���I�n�ˬd�Ҧ���L�I
		{
			int this_p = j;
			if (p_not_in_route[this_p])
			{
				//��X�̤p��k��
				if (can_p.size() < k) //����k�Ӫ�����i�h
				{
					//cout << "this p = " << this_p << endl;
					can_p.push_back(this_p);
				}
				else
				{
					// �h��̭���k �Ӥ�
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
	speed.resize(x.size()*1.8);
	for (int i = 0; i < speed.size(); i++)
	{
		//�쥻speed�O�ɳt�XKM�A���a�ϬO��M�A�ҥH�n��1000�A�ӥB�ɶ��O����A�ҥH�n��3600�A�ݨ��٬O����~�వ�o��
		//speed[i] = prob.avg_speed(); // ���O m/s
		speed[i] = prob.want_speed(); // 90 km/h
	}
	

}

/***********************************************************
random permutate()
���H�������H���ǦC(�S�ܮw)
�M��}�l��ܮw��i�h
(�̷�load�٦� time window�Atime window���ݪk�O��avg speed�h��ɶ��A�W�ɴN�P�_���U�@�x��)
�M��speed �����]�� ������
*************************************************************/

void CRandomInitialization::random_permutate(CIndividual *indv, const BProblem &prob) const
{
	/*
	�H���Ƨǫ�̾ڨ��l�����Mtime window �h��ܮw
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
		//�쥻speed�O�ɳt�XKM�A���a�ϬO��M�A�ҥH�n��1000�A�ӥB�ɶ��O����A�ҥH�n��3600�A�ݨ��٬O����~�వ�o��
		//speed[i] = prob.avg_speed(); // ���O m/s
		speed[i] = prob.want_speed(); // 90 km/h
	}
}
//---------------------------------------------------------------------------

/***********************************************************
random permutate_nd()
�H�������H���ǦC(�S�ܮw)
nd = no depot
�M��speed �����]�� ������
*************************************************************/

void CRandomInitialization::random_permutate_nd(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &x = indv->vars();
	const vector<Node> & node = prob.node();
	CIndividual::TDecVec &routes = indv->routes();
	const vector<vector<double>> &distance = prob.dis();
	x.resize(prob.num_node() - 1); // ���ݭn�ܮw�����I�A�ҥH�N���ͧǦC�ӼƴN�n(prob.num_node()-1)
	for (size_t i = 0; i < x.size(); i += 1)
	{
		x[i] = i + 1;
	}

	random_shuffle(x.begin(), x.end());
	routes = x;
	/*cout << "shuffle ---" << endl;
	cout << "Please Enter to continue..." << endl; getchar();*/

	//swap(x[0], *find(x.begin(), x.end(), prob.depot())); // ����depot �F

	/*cout << "Permutate : " << endl;

	for (int i = 0; i < x.size(); i++)
	{
	cout << x[i] << " ";
	}
	cout << endl;
	cout << "random_permutate finish ---" << endl;
	cout << "Please Enter to continue..." << endl; getchar();*/

	//----- set up the initial speed (�n���٤��γ]�w)-----
	CIndividual::TObjVec & speed = indv->speed();
	speed.resize(x.size()*1.8);
	for (int i = 0; i < speed.size(); i++)
	{
		//�쥻speed�O�ɳt�XKM�A���a�ϬO��M�A�ҥH�n��1000�A�ӥB�ɶ��O����A�ҥH�n��3600�A�ݨ��٬O����~�వ�o��
		//speed[i] = prob.avg_speed();
		speed[i] = prob.want_speed(); // 90 km/h
	}
	//for (int i = 0; i < speed.size(); i++) // �ˬd�o�̦��S���ʨ�t��
	//{
	//	if (speed[i] != 20.9294 && speed[i] != 15.3303)
	//	{
	//		cout << "i = " << i << endl;
	//		cout << "ini2 this speed is " << speed[i] << endl; getchar();
	//	}
	//}

}

//---------------------------------------------------------------------------
/***********************************************************
random permutate_nd()
�H�������H���ǦC(�S�ܮw)
nd = no depot
�M��speed �����]�� want_speed 
��ɳt90�٨줣�F���I�������n�Ʀb�e��
*************************************************************/
void CRandomInitialization::rand_permutate_nd_time(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &x = indv->vars();
	const vector<Node> & node = prob.node();
	CIndividual::TDecVec &routes = indv->routes();
	const vector<vector<double>> &distance = prob.dis();
	x.resize(prob.num_node() - 1); // ���ݭn�ܮw�����I�A�ҥH�N���ͧǦC�ӼƴN�n(prob.num_node()-1)
	vector<int> late_p; // �x�s����߶}�l�A�Ȫ��I
	//cout << "want speed = " << prob.want_speed() << endl;
	// find late service customer
	for (int i = 1; i < prob.num_node(); i += 1)
	{
		if (distance[prob.depot()][i] / prob.want_speed() <= node[i].ready_time - node[prob.depot()].ready_time)
		{
			//cout << "i = " << i << endl;
			late_p.push_back(i);
		}
	}
	//cout << late_p.size() << endl; getchar();
	for (size_t i = 0; i < x.size(); i += 1)
	{
		x[i] = i + 1;
	}
	bool check = false;
	while (!check)
	{
		check = true;
		random_shuffle(x.begin(), x.end());
		for (int i = 0; i < late_p.size(); i++)
		{
			if (x[0] == late_p[i])
			{
				check = false;
				break;
			}
		}
	}
	
	routes = x;
	/*cout << "shuffle ---" << endl;
	cout << "Please Enter to continue..." << endl; getchar();*/

	//swap(x[0], *find(x.begin(), x.end(), prob.depot())); // ����depot �F

	/*cout << "Permutate : " << endl;

	for (int i = 0; i < x.size(); i++)
	{
	cout << x[i] << " ";
	}
	cout << endl;
	cout << "random_permutate finish ---" << endl;
	cout << "Please Enter to continue..." << endl; getchar();*/

	//----- set up the initial speed (�n���٤��γ]�w)-----
	CIndividual::TObjVec & speed = indv->speed();
	speed.resize(x.size()*1.3);
	for (int i = 0; i < speed.size(); i++)
	{
		//�쥻speed�O�ɳt�XKM�A���a�ϬO��M�A�ҥH�n��1000�A�ӥB�ɶ��O����A�ҥH�n��3600�A�ݨ��٬O����~�వ�o��
		//speed[i] = prob.avg_speed();
		speed[i] = prob.want_speed(); // 90 km/h
	}
	//for (int i = 0; i < speed.size(); i++) // �ˬd�o�̦��S���ʨ�t��
	//{
	//	if (speed[i] != 20.9294 && speed[i] != 15.3303)
	//	{
	//		cout << "i = " << i << endl;
	//		cout << "ini2 this speed is " << speed[i] << endl; getchar();
	//	}
	//}

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
		this->KNN(&(*pop)[i], prob);
		//this->random_permutate_nd(&(*pop)[i], prob);
		//if (i < pop->size()*0.5)
		//{
		//	this->KNN(&(*pop)[i], prob);
		//}
		//else
		//{
		//	//this->rand_permutate_nd_time(&(*pop)[i], prob);
		//	this->random_permutate_nd(&(*pop)[i], prob);
		//}
		//cout << "sol = " << (*pop)[i].objs()[0] << endl;
	}
}
