#include "alg_refinement.h"
#include "alg_individual.h"
#include "problem_self.h"
#include "aux_math.h"
#include <vector>

using namespace std;

bool CNearestNeighborRefinement::operator()(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();

	//Nearest Neibor Local Optimal--------------------------------------------------- 
	double NN_rate = 1.0;
	double NN_now = MathAux::random(0.0, 1.0);
	size_t head = 1, tail = 1;
	for (size_t i = 1; NN_now <= NN_rate && i < routes.size(); i++)
	{
		if ( routes[i] == prob.depot())
		{
			tail = i;
			for (size_t j = head; j < tail-1; j++)
			{
				size_t target = j;
				double local_dis = distance[routes[j - 1] - 1][routes[j] - 1];

				for (size_t k = j + 1; k < tail; k++)
				{
					double temp_dis = distance[routes[j - 1] - 1][routes[k] - 1];
					if (temp_dis < local_dis)
					{
						local_dis = temp_dis;
						target = k;
					}
				}
				swap(routes[j], routes[target]);
			}
			head = tail + 1;
		}
	}

	return true;
}
//-----------------------------------------------------------------------------------------
bool CRandomNearestNeighborRefinement::operator()(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();

	//Random Routes Nearest Neibor Local Optimal----------Select some routes to NN
	size_t NN_rate = 20, NN_now = rand() % 100;
	size_t head = 1, tail = 1;
	for (size_t i = 1; i < routes.size(); i++)
	{
		NN_now = rand() % 100;
		if (routes[i] == prob.depot())
		{
			tail = i;
			if (NN_now < NN_rate)
			{
				head = tail + 1;
				continue;
			}
			for (size_t j = head; j < tail - 1; j++)
			{
				size_t target = j;
				double local_dis = distance[routes[j - 1] - 1][routes[j] - 1];

				for (size_t k = j + 1; k < tail; k++)
				{
					double temp_dis = distance[routes[j - 1] - 1][routes[k] - 1];
					if (temp_dis < local_dis)
					{
						local_dis = temp_dis;
						target = k;
					}
				}
				swap(routes[j], routes[target]);
			}
			head = tail + 1;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------------------

bool CPartialNearestNeighborRefinement::operator()(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();

	//Random Routes Nearest Neibor Local Optimal----------Select some part of routes to NN
	size_t head = 1, tail = 1;
	for (size_t i = 1; i < routes.size(); i++)
	{
		if (routes[i] == prob.depot())
		{
			tail = i;
			
			size_t rand_head = (rand() % (tail - head)) + head, rand_tail = (rand() % (tail - head) + head);
			if (rand_head > rand_tail) swap(rand_head, rand_tail);
			
			for (size_t j = rand_head; j <= rand_tail; j++)
			{
				size_t target = j;
				double local_dis = distance[routes[j - 1] - 1][routes[j] - 1];
				for (size_t k = j+1; k <= rand_tail; k++)
				{
					double temp_dis = distance[routes[j - 1] - 1][routes[k] - 1];
					if (temp_dis < local_dis)
					{
						local_dis = temp_dis;
						target = k;
					}
				}
				swap(routes[j], routes[target]);
			}
			head = tail + 1;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------------------

bool CReDispatchRefinement::operator()(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();


	//dispatch customer to match the limit---------------------------------------------------------------------------------
	if (indv->num_vehicles() > prob.num_vehicles())
	{
		/*
		開始調整過多的車輛數
		把車子拆開成1台1台來處理
		*/
		size_t num_v = indv->num_vehicles();

		while (num_v > prob.num_vehicles())
		{
			vector <double> v_load;
			//cout << "vnum = " << num_v << endl;
			double min_load = 1e9, load = 0.0;
			size_t index = 0, num = 0, min_cus = 1e9, num_customers = 0; //找最少的那台的index，從0開始

			vector <vector<size_t>> r;
			vector <size_t> r_tmp;
			//找出最小load的那個車
			for (size_t i = 1; i < routes.size(); i++)
			{
				if (routes[i] == prob.depot())
				{
					r.push_back(r_tmp);
					//cout << "r size = " << r.size() << endl;
					r_tmp.clear();
					//cout << "r_tmp size = " << r_tmp.size() << endl;
					//cout << "load = " << load << endl;
					v_load.push_back(load);
					/*if (num_customers < min_cus)
					{
					min_cus = num_customers;
					index = num;
					}*/
					if (load < min_load)
					{
						min_load = load;
						index = num;
					}
					load = 0.0;
					num_customers = 0;
					num++;
				}
				else
				{
					load += n[routes[i] - 1].demand;
					num_customers++;
					r_tmp.push_back(routes[i]);
				}
			}

			//看每台load有多少
			/*cout << "load original:" << endl;
			for (int i = 0; i < v_load.size(); i++)
			{
			cout << "v_load[" << i << "] = " << v_load[i] << endl;
			}
			cout << endl;*/

			/*cout << "\noriginal :\n";
			for (int i = 0; i < r.size(); i++)
			{
			for (int j = 0; j < r[i].size(); j++)
			{
			cout << r[i][j] << ' ';
			}
			cout << endl;
			}*/
			//cout << "min_cus = " << min_cus << endl;
			//cout << "min load = " << min_load << endl;
			//cout << "r size = " << r.size() << endl;
			//cout << "v_load size = "<< v_load.size() << endl;
			//system("pause");
			//證明了不一定分割出來的一定很小
			bool check = false;
			for (size_t i = 0; i < r[index].size(); i++)
			{
				check = false;
				//cout << "r[index][i] = " << r[index][i] << endl;
				//cout << "i = " << i << endl;
				//cout << "r size = " << r.size() << endl;
				//cout << "v_load size = " << v_load.size() << endl;
				for (int j = index, k = index; j >= 0 || k< num; j--, k++)
				{
					//cout << "j = " << j << endl;
					if (j != index && j >= 0)
					{
						if (v_load[j] + n[r[index][i] - 1].demand <= prob.maxload())
						{
							r[j].push_back(r[index][i]);
							v_load[j] += n[r[index][i] - 1].demand;
							//cout << "vload[" << j << "]= " << v_load[j] << endl;
							check = true;
							break;
						}
					}
					//if (j < 0) j = num;
					if (k != index && k < num)
					{
						if (v_load[k] + n[r[index][i] - 1].demand <= prob.maxload())
						{
							//r[k].push_back(r[index][i]);
							r[k].insert(r[k].begin(), r[index][i]);
							//swap(r[k][0], r[k][r[k].size() - 1]);
							v_load[k] += n[r[index][i] - 1].demand;
							//cout << "vload[" << k << "]= " << v_load[k] << endl;
							check = true;
							break;
						}
					}
					//if (k == num) k = -1;
				}
				if (check == false) break;
			}
			//system("pause");
			//重新創造新的routes
			if (check)
			{
				//cout << "refine OK " << endl;
				routes.clear();
				routes.push_back(prob.depot());
				//cout << "0 ";
				for (size_t i = 0; i < num; i++)
				{
					if (i != index)
					{
						for (size_t j = 0; j < r[i].size(); j++)
						{
							routes.push_back(r[i][j]);
							//cout << r[i][j] << ' ';
						}
						//cout << "0 ";
						routes.push_back(prob.depot());
					}
				}
				num_v--;
				/*cout << "\nTrue : \n";
				for (int i = 0; i < routes.size(); i++) cout << routes[i] << ' ';
				cout << endl;*/
				//cout << " num v = " << num_v << endl;
				indv->set_num_vehicles(num_v);
				//system("pause");
			}
			else
			{
				//cout << "re dispatch fail" << endl;
				break;
			}
		}
	}


	return true;
}
//------------------------------------------------------------------------------------------
bool CNEHRefinement::operator()(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();

	//BestInsert--------------------------------------------------- 
	double BI_rate = 1.0;
	double BI_now = MathAux::random(0.0, 1.0);
	size_t head = 1, tail = 1;
	for (size_t i = 1; BI_now <= BI_rate && i < routes.size(); i++)
	{
		if (routes[i] == prob.depot())
		{
			//head 會是現在第一個不是倉庫的index,tail 是下一個倉庫的index
			tail = i;
			//先把頭換成離倉庫最近的
			double local_dis = distance[routes[i] - 1][routes[head] - 1];
			size_t target = head;
			for (size_t j = head+1; j < tail; j++)
			{
				double temp_dis = distance[routes[i] - 1][routes[j] - 1];

				if (temp_dis < local_dis)
				{
					local_dis = temp_dis;
					target = j;
				}
			}
			swap(routes[head], routes[target]);
			//-------------------------------------------------------------

			for (size_t j = head+1; j < tail; j++)
			{
				size_t target = 0;
				double min_dis = 1e9;
				size_t now_cus = routes[j];
				for (size_t k = j; k >= head; k--)
				{
					routes.erase(routes.begin() + j);
					routes.insert(routes.begin() + k,now_cus);

					double total_dis = distance[prob.depot() - 1][routes[head] - 1];
					for (size_t h = head+1; h <= j; h++)
					{
						if ( h == tail - 1)
						{
							total_dis += distance[prob.depot() - 1][routes[h] - 1];
						}
						
						total_dis += distance[routes[h - 1] - 1][routes[h] - 1];
					}
					if (total_dis < min_dis)
					{
						min_dis = total_dis;
						target = k;
					}
					routes.erase(routes.begin() + k);
					routes.insert(routes.begin() + j, now_cus);
				}
				routes.erase(routes.begin() + j);
				routes.insert(routes.begin() + target, now_cus);
			}
			head = tail + 1;
		}
	}
	return true;
}
//-------------------------------------------------------------------------------------------

bool CRandomNEHRefinement::operator()(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();

	//Random NEH---------------Random route NEH------------------------------------ 
	size_t NEH_rate = 100, NEH_now = rand() % 100;
	size_t head = 1, tail = 1;
	for (size_t i = 1;  i < routes.size(); i++)
	{
		NEH_now = rand() % 100;
		if (routes[i] == prob.depot())
		{
			//head 會是現在第一個不是倉庫的index,tail 是下一個倉庫的index
			tail = i;
			if (NEH_now < NEH_rate) {
				head = tail + 1;
				continue;
			}
			//先把頭換成離倉庫最近的
			double local_dis = distance[routes[i] - 1][routes[head] - 1];
			size_t target = head;
			for (size_t j = head + 1; j < tail; j++)
			{
				double temp_dis = distance[routes[i] - 1][routes[j] - 1];

				if (temp_dis < local_dis)
				{
					local_dis = temp_dis;
					target = j;
				}
			}
			swap(routes[head], routes[target]);
			//-------------------------------------------------------------
			for (size_t j = head + 1; j < tail; j++)
			{
				size_t target = 0;
				double min_dis = 1e9;
				size_t now_cus = routes[j];
				for (size_t k = j; k >= head; k--)
				{
					routes.erase(routes.begin() + j);
					routes.insert(routes.begin() + k, now_cus);

					double total_dis = distance[prob.depot() - 1][routes[head] - 1];
					for (size_t h = head + 1; h <= j; h++)
					{
						if (h == tail - 1)
						{
							total_dis += distance[prob.depot() - 1][routes[h] - 1];
						}

						total_dis += distance[routes[h - 1] - 1][routes[h] - 1];
					}
					if (total_dis < min_dis)
					{
						min_dis = total_dis;
						target = k;
					}
					routes.erase(routes.begin() + k);
					routes.insert(routes.begin() + j, now_cus);
				}
				routes.erase(routes.begin() + j);
				routes.insert(routes.begin() + target, now_cus);
			}
			head = tail + 1;
		}
	}
	return true;
}

//-------------------------------------------------------------------------------------------

bool CPartialNEHRefinement::operator()(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();

	//Random NEH---------------Random route NEH------------------------------------ 
	size_t head = 1, tail = 1;
	for (size_t i = 1; i < routes.size(); i++)
	{
		if (routes[i] == prob.depot())
		{
			//head 會是現在第一個不是倉庫的index,tail 是下一個倉庫的index
			tail = i;
			//隨機某段-----------------------
			size_t rand_head = (rand() % (tail - head)) + head, rand_tail = (rand() % (tail - head) + head);
			if (rand_head > rand_tail) swap(rand_head, rand_tail);
			//-----------------------------
			for (size_t j = rand_head; j <= rand_tail; j++)
			{
				size_t target = 0;
				double min_dis = 1e9;
				size_t now_cus = routes[j];
				for (size_t k = j; k >= rand_head; k--)
				{
					routes.erase(routes.begin() + j);
					routes.insert(routes.begin() + k, now_cus);

					double total_dis = distance[routes[rand_head-1] - 1][routes[rand_head] - 1];
					for (size_t h = rand_head + 1; h <= j; h++)
					{
						if (h == j)
						{
							total_dis += distance[routes[rand_tail+1] - 1][routes[h] - 1];
						}

						total_dis += distance[routes[h - 1] - 1][routes[h] - 1];
					}
					if (total_dis < min_dis)
					{
						min_dis = total_dis;
						target = k;
					}
					routes.erase(routes.begin() + k);
					routes.insert(routes.begin() + j, now_cus);
				}
				routes.erase(routes.begin() + j);
				routes.insert(routes.begin() + target, now_cus);
			}
			head = tail + 1;
		}
	}
	return true;
}

//-------------------------------------------------------------------------------------------

bool CPartNEH2Object::operator()(CIndividual *indv, const BProblem &prob,const int obj1_rate, const double normal_dis, const double normal_emi) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();
	const double obj1 = (double)obj1_rate / 100, obj2 = 1 - obj1;
	const double m = 2.61, FCR_full_ = 0.390*10, FCR_empty_ = 0.296*1; //m = CER = CO2 rate
	//indv->set_eva(0); //計算evaluate次數
	//Partial NEH 2Object-----已經改成雙目標---------------------------------------------- 
	size_t head = 1, tail = 1;
	for (size_t i = 1; i < routes.size(); i++)
	{
		if (routes[i] == prob.depot())
		{
			//head 會是現在第一個不是倉庫的index,tail 是下一個倉庫的index
			tail = i;
			//隨機某段-----------------------
			size_t rand_head = (rand() % (tail - head)) + head, rand_tail = (rand() % (tail - head) + head);
			if (rand_head > rand_tail) swap(rand_head, rand_tail);
			//-----------------------------
			for (size_t j = rand_head; j <= rand_tail; j++)
			{
				size_t target = 0;
				double min_value = 1e9;
				size_t now_cus = routes[j];
				for (size_t k = j; k >= rand_head; k--)
				{
					routes.erase(routes.begin() + j);
					routes.insert(routes.begin() + k, now_cus);
					double now_load = n[routes[rand_head] - 1].demand;
					double total_dis = distance[routes[rand_head - 1] - 1][routes[rand_head] - 1];
					double total_value = m *((FCR_full_ - FCR_empty_) * now_load / prob.maxload())  * total_dis;
					for (size_t h = rand_head + 1; h <= j; h++)
					{
						now_load += n[routes[h] - 1].demand;
						total_dis += distance[routes[h - 1] - 1][routes[h] - 1];
						total_value += m * ((FCR_full_ - FCR_empty_) * now_load / prob.maxload())  * total_dis;
					}
					total_dis += distance[routes[rand_tail + 1] - 1][routes[j] - 1];
					total_value += m * FCR_empty_ * total_dis;
					total_value = obj2 * total_value;
					total_value += obj1 * total_dis;

					if (total_value < min_value)
					{
						min_value = total_value;
						target = k;
					}
					//indv->add_eva();
					routes.erase(routes.begin() + k);
					routes.insert(routes.begin() + j, now_cus);
				}
				routes.erase(routes.begin() + j);
				routes.insert(routes.begin() + target, now_cus);
			}
			head = tail + 1;
		}
	}
	//cout << "PNEH - try = " << try_num << endl;
	return true;
}
//-------------------------------------------------------------------------------------------
bool CPartNN2Object::operator()(CIndividual *indv, const BProblem &prob,const int obj1_rate, const double normal_dis, const double normal_emi) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();
	const double obj1 = (double)obj1_rate / 100, obj2 = 1 - obj1;
	const double m = 2.61, FCR_full_ = 0.390*10, FCR_empty_ = 0.296*1; //m = CER = CO2 rate
	//indv->set_eva(0);
	//Partial Nearest Neibor 2Object----------Select some part of routes to NN
	size_t head = 1, tail = 1;
	for (size_t i = 1; i < routes.size(); i++)
	{
		if (routes[i] == prob.depot())
		{
			tail = i;

			size_t rand_head = (rand() % (tail - head)) + head, rand_tail = (rand() % (tail - head) + head);
			if (rand_head > rand_tail) swap(rand_head, rand_tail);

			for (size_t j = rand_head; j <= rand_tail; j++)
			{
				size_t target = j;
				//double local_dis = distance[routes[j - 1] - 1][routes[j] - 1];
				double local_value = obj1 * distance[routes[j - 1] - 1][routes[j] - 1] + obj2 * m *((FCR_full_ - FCR_empty_) * (prob.maxload()-n[routes[j - 1] - 1].demand) / prob.maxload())  * distance[routes[j - 1] - 1][routes[j] - 1];

				for (size_t k = j + 1; k <= rand_tail; k++)
				{
					//double temp_dis = distance[routes[j - 1] - 1][routes[k] - 1];
					double temp_value = obj1 * distance[routes[j - 1] - 1][routes[k] - 1] + obj2 * m *((FCR_full_ - FCR_empty_) * (prob.maxload() - n[routes[j - 1] - 1].demand) / prob.maxload())  * distance[routes[j - 1] - 1][routes[k] - 1];
					//indv->add_eva();
					if (temp_value < local_value)
					{
						local_value = temp_value;
						target = k;
					}
				}
				swap(routes[j], routes[target]);
			}
			head = tail + 1;
		}
	}
	//cout << "PNN - try = " << try_num << endl;
	return true;
}
//-------------------------------------------------------------------------------------------
bool CTwoOpt2Object::operator()(CIndividual *indv, const BProblem &prob, const int obj1_rate, const double normal_dis, const double normal_emi, const double Min_dis, const double Min_emi) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();
	const double obj1 = (double)obj1_rate / 100, obj2 = 1 - obj1;
	const double m = 2.61, FCR_full_ = 0.390*10, FCR_empty_ = 0.296*1; //m = CER = CO2 rate
	indv->set_eva(0);

	//2-OPT------------------------------------------------------------
	size_t head = 1, tail = 1;
	for (size_t i = 1; i < routes.size(); i++)
	{
		if (routes[i] == prob.depot())
		{
			tail = i;
			//cout << "head = " << head << " tail = " << tail << endl;
			//best_dis 先弄成原路線distance
			double best_dis = distance[routes[head - 1] - 1][routes[head] - 1],
				now_load = n[routes[head] - 1].demand,
				best_value =  m * ((FCR_full_ - FCR_empty_) * now_load / prob.maxload())*best_dis;
			//cout << "origial route = ";
			for (size_t j = head + 1; j < tail; j++)
			{
				//cout << routes[j - 1] << ' ';
				best_dis += distance[routes[j - 1] - 1][routes[j] - 1];
				now_load += n[routes[j] - 1].demand;
				best_value +=  m * ((FCR_full_ - FCR_empty_) * now_load / prob.maxload()) * best_dis;
			}
			best_dis += distance[routes[tail - 1] - 1][routes[tail] - 1];
			best_value += m * FCR_empty_ * best_dis;
			best_value = obj2 * (best_value - Min_emi) / normal_emi;
			best_value += obj1 * (best_dis-Min_dis)/normal_dis;
			//cout << routes[tail - 1] << endl;
			//------------------------------------
			vector<int>best_route;
			for (int j = 0; j < tail - head - 1; j++)
			{
				//cout << "j = " << j << endl;
				for (int k = j + 1; k < tail - head; k++)
				{
					//cout << "k = " << k << endl;
					//製造new_route(轉成2-opt後的)
					vector<int> new_route;
					//cout << "new = ";
					for (int c = 0; c <= j - 1; c++) {
						//cout << routes[c + head] << ' ';
						new_route.push_back(routes[c + head]);
					}
					//cout << endl;
					int dec = 0;
					for (int c = j; c <= k; c++) {
						//cout << routes[k - dec + head] << ' ';
						new_route.push_back(routes[k - dec + head]);
						dec++;
					}
					//cout << endl;
					for (int c = k + 1; c < tail - head; c++) {
						//cout << routes[c + head] << ' ';
						new_route.push_back(routes[c + head]);
					}
					//cout << endl;
					//------------------------------
					//算new_route的distance
					double new_dis = distance[routes[head - 1] - 1][new_route[0] - 1],
						   new_load = n[new_route[0] - 1].demand,
						   new_value =  m * ((FCR_full_ - FCR_empty_) * new_load / prob.maxload())*new_dis;

					//cout << "tail - head = " << tail - head << endl;
					//cout << "new size = " << new_route.size() << endl;
					for (int c = 1; c < tail - head; c++) {

						new_dis += distance[new_route[c - 1] - 1][new_route[c] - 1];
						new_load += n[new_route[c] - 1].demand;
						new_value +=  m * ((FCR_full_ - FCR_empty_) * new_load / prob.maxload())*new_dis;
					}
					new_dis += distance[routes[tail] - 1][new_route[tail - head - 1] - 1];
					new_value += m * FCR_empty_ * new_dis;
					new_value = obj2 * (new_value - Min_emi) / normal_emi;
					new_value += obj1 * (new_dis-Min_dis)/normal_dis;
					indv->add_eva();
					if (new_value < best_value) {
						best_value = new_value;
						best_route.clear();
						//cout << "tail - head = " << tail - head << endl;
						//cout << "new_route = ";
						for (int c = 0; c < tail - head; c++) {
							//cout << new_route[c] << ' ';
							routes[c + head] = new_route[c];
							//best_route.push_back(new_route[c]);
						}
						//cout << endl;
						//system("pause");
					}
				}
				//cout << "best route size = " << best_route.size() << endl;
				//cout << "tail - head = " << tail - head << endl;
				//for (int c = 0; c < tail - head; c++) {
				//	//cout << new_route[c] << ' ';
				//	routes[c+head] = best_route[c];
				//}
			}
			head = tail + 1;
		}
	}
	//cout << "2OPT - try = " << try_num << endl;
	return true;
}

//------------------------------------------------------------------------------------------

bool CFartoNearSort::operator()(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();

	//Far to Near Sort--------------------------------------------------- 
	double NN_rate = 1.0;
	double NN_now = MathAux::random(0.0, 1.0);
	size_t head = 1, tail = 1;
	for (size_t i = 1; NN_now <= NN_rate && i < routes.size(); i++)
	{
		if (routes[i] == prob.depot())
		{
			tail = i;
			for (size_t j = head; j < tail - 1; j++)
			{
				size_t target = j;
				double local_dis = distance[prob.depot() - 1][routes[j] - 1];

				for (size_t k = j + 1; k < tail; k++)
				{
					double temp_dis = distance[prob.depot() - 1][routes[k] - 1];
					if (temp_dis > local_dis)//<就是小到大，>就是大到小
					{
						local_dis = temp_dis;
						target = k;
					}
				}
				swap(routes[j], routes[target]);
			}
			head = tail + 1;
		}
	}

	return true;
}

//------------------------------------------------------------------------------------------
bool COnePointOpt::operator()(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<vector<double>> & distance = prob.dis();
	indv->set_eva(0);

	// --- One Point Opt-----------find one point to insert to its original route (best)
	size_t rand_num = 0;
	while (routes[rand_num] == prob.depot() ){// create rand num in routes
		rand_num = rand() % routes.size();
	}

	// --- search the left and right depot's indexes
	size_t left_depot = 1, right_depot = 1,now = rand_num;
	while (routes[now] != prob.depot()){
		now--;
	}
	left_depot = now;
	now = rand_num;
	while (routes[now] != prob.depot()) {
		now++;
	}
	right_depot = now;

	// --- insert every position in its route
	size_t target = rand_num, now_cus = routes[rand_num];
	double min_dis = 1e9;

	for (size_t i = left_depot + 1;  i < right_depot; i++)
	{
		routes.erase(routes.begin() +  rand_num);
		routes.insert(routes.begin() +  i, now_cus);

		double total_dis = distance[prob.depot() - 1][routes[left_depot+1] - 1];
		for (size_t h = left_depot + 2; h < right_depot; h++)
		{
			if (h == right_depot - 1)
			{
				total_dis += distance[prob.depot() - 1][routes[h] - 1];
			}
			total_dis += distance[routes[h - 1] - 1][routes[h] - 1];
		}
		indv->add_eva();
		if (total_dis < min_dis)
		{
			min_dis = total_dis;
			target = i;
		}
		routes.erase(routes.begin() + i);
		routes.insert(routes.begin() + rand_num, now_cus);
	}
	routes.erase(routes.begin() + rand_num);
	routes.insert(routes.begin() + target, now_cus);

	return true;
}

//------------------------------------------------------------------------------------------

bool CAllPointOpt::operator()(CIndividual *indv, const BProblem &prob,const int obj1_rate, const double normal_dis, const double normal_emi, const double Min_dis, const double Min_emi) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();
	const double obj1 = (double)obj1_rate / 100, obj2 = 1 - obj1;
	const double m = 2.61, FCR_full_ = 0.390 * 10, FCR_empty_ = 0.296 * 1; //m = CER = CO2 rate
	const double wrong_value = 1e9;
	indv->set_eva(0);

	// --- All Point Opt-----------find one point to insert to its original route (best) and try it all point

	size_t left_depot = 0, right_depot = 1;

	for (size_t cus = 0; cus < routes.size(); cus++)
	{
		// --- search the right depot's indexes
		if (routes[cus] == prob.depot())
		{
			right_depot = cus;
			// --- insert every position in its route
			size_t before_ind = 0, after_ind = 0,change_cus = routes[left_depot+1];
			double min_dis_in_route = wrong_value, min_value_in_route = wrong_value;

			for (size_t i = left_depot + 1; i < right_depot; i++)
			{
				size_t now_cus = routes[i];
				//double min_dis_in_cus= wrong_value, min_value_in_cus = wrong_value;
				//for (size_t q = left_depot + 1; q < right_depot; q++)
				//{
				//	cout << routes[q] << ' ';
				//}
				//cout << endl; //getchar();

				for (size_t j = left_depot + 1; j < right_depot; j++)
				{
					if (i == j) continue;
					
					routes.erase(routes.begin() + i);
					routes.insert(routes.begin() + j, now_cus);

					//cout << "after insert" << endl;
					//for (size_t q = left_depot + 1; q < right_depot; q++)
					//{
					//	cout << routes[q] << ' ';
					//}
					//cout << endl; //getchar();

					double total_dis = distance[prob.depot() - 1][routes[left_depot + 1] - 1],
						   now_load = n[routes[left_depot + 1] - 1].demand,
						   total_value = m * ((FCR_full_ - FCR_empty_) * now_load / prob.maxload())*total_dis;
					for (size_t h = left_depot + 2; h < right_depot; h++)
					{
						total_dis += distance[routes[h - 1] - 1][routes[h] - 1];
						now_load += n[routes[j] - 1].demand;
						total_value += m * ((FCR_full_ - FCR_empty_) * now_load / prob.maxload()) * total_dis;
					}
					total_dis += distance[prob.depot() - 1][routes[right_depot-1] - 1];
					total_value += m * FCR_empty_ * total_dis;
					total_value = obj2 * (total_value - Min_emi) / normal_emi;
					total_value += obj1 * (total_dis - Min_dis) / normal_dis;

					indv->add_eva();
					if (total_value < min_value_in_route)
					{
						min_value_in_route = total_value;
						change_cus = now_cus;
						before_ind = i;
						after_ind = j;
					}
					routes.erase(routes.begin() + j);
					routes.insert(routes.begin() + i, now_cus);
				}
				
			}
			routes.erase(routes.begin() + before_ind);
			routes.insert(routes.begin() + after_ind, change_cus);

			//cout << "really insert" << endl;
			//for (size_t q = left_depot + 1; q < right_depot; q++)
			//{
			//	cout << routes[q] << ' ';
			//}
			//cout << endl; //getchar();

			left_depot = right_depot;
		}

	}
	//cout << "APO num = " << indv->num_evaluate() << endl;

	return true;
}

//------------------------------------------------------------------------------------------
bool COnePointOptGlobal::operator()(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();
	int try_num = 0;

	//One Point Opt-----------find one point to insert to every cut optimal (global)
	size_t rand_num = 0;
	while (routes[rand_num] == prob.depot()) {
		rand_num = rand() % routes.size();
		if (rand_num > 0 && rand_num < routes.size() - 1)
		{
			if (routes[rand_num - 1] == prob.depot() && routes[rand_num + 1] == prob.depot()) {
				cout << "Oops -- problem deliver one customer" << endl;
				rand_num = 0;
				continue;
			}
		}
	}
	//cout << "routes rand = " << routes[rand_num] << endl;
	size_t left_depot = 0, right_depot = 0, now = rand_num, ori_left = rand_num, ori_right =  rand_num, target = rand_num, now_cus = routes[rand_num];
	while (routes[ori_left] != prob.depot()) {
		ori_left--;
	}
	while (routes[ori_right] != prob.depot()) {
		ori_right++;
	}
	//計算原本路線扣除這點的distance
	double dis_no_target = distance[prob.depot() - 1][routes[ori_left + 1] - 1];
	for (size_t i = ori_left + 1; i < ori_right; i++) {
		if (i == rand_num) {
			if (i == ori_right - 1) {
				dis_no_target += distance[prob.depot() - 1][routes[i-1] - 1];
			}
			continue;
		}
		if (i == ori_right - 1)
		{
			dis_no_target += distance[prob.depot() - 1][routes[i] - 1];
		}
		dis_no_target += distance[routes[i - 1] - 1][routes[i] - 1];
	}

	while (right_depot != routes.size()-1) {
		//set好倉庫兩端(某條路線)
		left_depot = right_depot;
		right_depot++;
		//cout << "left = " << left_depot << endl;
		double load = 0.0;
		while (routes[right_depot] != prob.depot()) {
			load += n[routes[right_depot]-1].demand; //計算這條路線目前的load
			right_depot++;
		}
		//cout << "load = " << load << endl;
		if (load + n[routes[rand_num]-1].demand > prob.maxload()) { //這條路線不能多載這個貨物就不用算了
			//cout << "continue" << endl;
			continue;
		}

		double min_dis = 1e9;

		for (size_t i = left_depot + 1; i < right_depot; i++)
		{
			routes.erase(routes.begin() + rand_num);
			routes.insert(routes.begin() + i, now_cus);

			double total_dis = distance[prob.depot() - 1][routes[left_depot + 1] - 1] + dis_no_target;
			for (size_t h = left_depot + 2; h < right_depot; h++)
			{
				if (h == right_depot - 1)
				{
					total_dis += distance[prob.depot() - 1][routes[h] - 1];
				}

				total_dis += distance[routes[h - 1] - 1][routes[h] - 1];
			}
			if (total_dis < min_dis)
			{
				min_dis = total_dis;
				target = i;
			}
			try_num++;
			routes.erase(routes.begin() + i);
			routes.insert(routes.begin() + rand_num, now_cus);
		}
	}
	
	routes.erase(routes.begin() + rand_num);
	routes.insert(routes.begin() + target, now_cus);
	cout << "OPOG - try = " << try_num << endl;
	//cout << "target = " << target << endl;

	return true;
}

//------------------------------------------------------------------------------------------
bool CTwoOpt::operator()(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();

	//2-OPT------------------------------------------------------------
	size_t head = 1, tail = 1;
	for (size_t i = 1; i < routes.size(); i++)
	{
		if (routes[i] == prob.depot())
		{
			tail = i;
			//cout << "head = " << head << " tail = " << tail << endl;
			//best_dis 先弄成原路線distance
			double best_dis = distance[routes[head - 1] - 1][routes[head] - 1];
			//cout << "origial route = ";
			for (size_t j = head+1; j < tail; j++)
			{
				//cout << routes[j - 1] << ' ';
				best_dis += distance[routes[j - 1] - 1][routes[j] - 1];
			}
			best_dis += distance[routes[tail - 1] - 1][routes[tail] - 1];
			//cout << routes[tail - 1] << endl;
			//------------------------------------
			vector<int>best_route;
			for (int j = 0; j < tail - head - 1; j++)
			{
				//cout << "j = " << j << endl;
				for (int k = j + 1; k < tail - head; k++)
				{
					//cout << "k = " << k << endl;
					//製造new_route(轉成2-opt後的)
					vector<int> new_route;
					//cout << "new = ";
					for (int c = 0; c <= j-1; c++) {
						//cout << routes[c + head] << ' ';
						new_route.push_back(routes[c+head]);
					}
					//cout << endl;
					int dec = 0;
					for (int c = j; c <= k; c++) {
						//cout << routes[k - dec + head] << ' ';
						new_route.push_back(routes[k - dec+head]);
						dec++;
					}
					//cout << endl;
					for (int c = k + 1; c < tail-head; c++) {
						//cout << routes[c + head] << ' ';
						new_route.push_back(routes[c+head]);
					}
					//cout << endl;
					//------------------------------
					//算new_route的distance
					double new_dis = distance[routes[head - 1] - 1][new_route[0]- 1];
					//cout << "tail - head = " << tail - head << endl;
					//cout << "new size = " << new_route.size() << endl;
					for (int c = 1; c < tail - head; c++) {
						
						new_dis += distance[new_route[c-1] - 1][new_route[c] - 1];
					}
					new_dis += distance[routes[tail] - 1][new_route[tail-head-1] - 1];

					if (new_dis < best_dis) {
						best_dis = new_dis;
						best_route.clear();
						//cout << "tail - head = " << tail - head << endl;
						//cout << "new_route = ";
						for (int c = 0; c < tail - head; c++) {
							//cout << new_route[c] << ' ';
							routes[c+head] = new_route[c];
							//best_route.push_back(new_route[c]);
						}
						//cout << endl;
						//system("pause");
					}
				}
				//cout << "best route size = " << best_route.size() << endl;
				//cout << "tail - head = " << tail - head << endl;
				//for (int c = 0; c < tail - head; c++) {
				//	//cout << new_route[c] << ' ';
				//	routes[c+head] = best_route[c];
				//}
			}
			head = tail + 1;
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------
bool COnePointRelocation::operator()(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();
	/*size_t num_mr = rand() % 100;
	bool mr = true;
	if (num_mr >= 35) {
	mr = false;
	return mr;
	}*/

	//One Point Relocation-----------find one point to insert to every cut optimal (global and do it every point)
	/*把每個點都拿來插插看，插到不會再變好為止 */
	size_t rand_num = 0;
	while (routes[rand_num] == prob.depot()) {
		rand_num = rand() % routes.size();
		if (rand_num > 0 && rand_num < routes.size() - 1)
		{
			if (routes[rand_num - 1] == prob.depot() && routes[rand_num + 1] == prob.depot()) {
				cout << "Oops -- problem deliver one customer" << endl;
				rand_num = 0;
				continue;
			}
		}
	}
	//cout << "routes rand = " << routes[rand_num] << endl;
	size_t left_depot = 0, right_depot = 0, now = rand_num, ori_left = rand_num, ori_right = rand_num, target = rand_num, now_cus = routes[rand_num];
	while (routes[ori_left] != prob.depot()) {
		ori_left--;
	}
	while (routes[ori_right] != prob.depot()) {
		ori_right++;
	}
	//計算原本路線扣除這點的distance
	double dis_no_target = distance[prob.depot() - 1][routes[ori_left + 1] - 1];
	for (size_t i = ori_left + 1; i < ori_right; i++) {
		if (i == rand_num) {
			if (i == ori_right - 1) {
				dis_no_target += distance[prob.depot() - 1][routes[i - 1] - 1];
			}
			continue;
		}
		if (i == ori_right - 1)
		{
			dis_no_target += distance[prob.depot() - 1][routes[i] - 1];
		}
		dis_no_target += distance[routes[i - 1] - 1][routes[i] - 1];
	}

	while (right_depot != routes.size() - 1) {
		//set好倉庫兩端(某條路線)
		left_depot = right_depot;
		right_depot++;
		//cout << "left = " << left_depot << endl;
		double load = 0.0;
		while (routes[right_depot] != prob.depot()) {
			load += n[routes[right_depot] - 1].demand; //計算這條路線目前的load
			right_depot++;
		}
		//cout << "load = " << load << endl;
		if (load + n[routes[rand_num] - 1].demand > prob.maxload()) { //這條路線不能多載這個貨物就不用算了
																	   //cout << "continue" << endl;
			continue;
		}

		double min_dis = 1e9;

		for (size_t i = left_depot + 1; i < right_depot; i++)
		{
			routes.erase(routes.begin() + rand_num);
			routes.insert(routes.begin() + i, now_cus);

			double total_dis = distance[prob.depot() - 1][routes[left_depot + 1] - 1] + dis_no_target;
			for (size_t h = left_depot + 2; h < right_depot; h++)
			{
				if (h == right_depot - 1)
				{
					total_dis += distance[prob.depot() - 1][routes[h] - 1];
				}

				total_dis += distance[routes[h - 1] - 1][routes[h] - 1];
			}
			if (total_dis < min_dis)
			{
				min_dis = total_dis;
				target = i;
			}
			routes.erase(routes.begin() + i);
			routes.insert(routes.begin() + rand_num, now_cus);
		}
	}

	routes.erase(routes.begin() + rand_num);
	routes.insert(routes.begin() + target, now_cus);
	//cout << "target = " << target << endl;

	return true;
}

//------------------------------------------------------------------------------------------
bool CReArrangePermutation::operator()(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &routes = indv->routes();

	// re-arrange the permutation
	//要有這個才會重新設定indv->var(),不然沒改變真正的順序

	for (size_t i = 1, j = 1; i < routes.size(); i++)
	{
		if (routes[i] == prob.depot()){
			continue;
		}
		else
		{
			indv->vars()[j] = routes[i];
			j += 1;
		}
	}
	return true;
}