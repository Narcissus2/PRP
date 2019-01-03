#include "alg_refinement.h"
#include "alg_individual.h"
#include "problem_self.h"
#include "aux_math.h"
#include <vector>
#include <algorithm>

using namespace std;

bool CNearestNeighborRefinement::operator()(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();

	//Nearest Neibor Local Optimal--------------------------------------------------- 
	double NN_threshold = 1.0;
	double NN_now = MathAux::random(0.0, 1.0);
	size_t head = 1, tail = 1;

	if (NN_now > NN_threshold)
	{
		return false;
	}

	for (size_t i = 1; NN_now <= NN_threshold && i < routes.size(); i++)
	{
		if (routes[i] == prob.depot())
		{
			tail = i;
			for (size_t j = head; j < tail - 1; j++)
			{
				size_t target = j;
				double local_dis = distance[routes[j - 1]][routes[j]];

				for (size_t k = j + 1; k < tail; k++)
				{
					double temp_dis = distance[routes[j - 1]][routes[k]];
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
	const size_t NN_threshold = 20;
	size_t head = 1, tail = 1;
	bool check = false;
	for (size_t i = 1; i < routes.size(); i++)
	{
		size_t NN_now = rand() % 100;
		if (routes[i] == prob.depot())
		{
			tail = i;
			if (NN_now < NN_threshold)
			{
				head = tail + 1;
				continue;
			}
			check = true;
			for (size_t j = head; j < tail - 1; j++)
			{
				size_t target = j;
				double local_dis = distance[routes[j - 1]][routes[j]];

				for (size_t k = j + 1; k < tail; k++)
				{
					double temp_dis = distance[routes[j - 1]][routes[k]];
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
	if (check) return true;
	else return false;
}

//-----------------------------------------------------------------------------------------

bool CPartialNearestNeighborRefinement::operator()(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();

	//Random Routes Nearest Neibor Local Optimal----------Select some part of routes to NN
	size_t head = 1, tail = 1;
	bool check = false;
	for (size_t i = 1; i < routes.size(); i++)
	{
		if (routes[i] == prob.depot())
		{
			tail = i;

			size_t rand_head = (rand() % (tail - head)) + head, rand_tail = (rand() % (tail - head) + head);
			if (rand_head > rand_tail) swap(rand_head, rand_tail);

			for (size_t j = rand_head; j <= rand_tail; j++)
			{
				check = true;
				size_t target = j;
				double local_dis = distance[routes[j - 1]][routes[j]];
				for (size_t k = j + 1; k <= rand_tail; k++)
				{
					double temp_dis = distance[routes[j - 1]][routes[k]];
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
	return check;
}

//-----------------------------------------------------------------------------------------

// bool CReDispatchRefinement::operator()(CIndividual *indv, const BProblem &prob) const
// {
// 	CIndividual::TDecVec &routes = indv->routes();
// 	const vector<Node> & n = prob.node();
// 	const vector<vector<double>> & distance = prob.dis();


// 	//dispatch customer to match the limit---------------------------------------------------------------------------------
// 	if (indv->num_vehicles() > prob.num_vehicles())
// 	{
// 		/*
// 		開始調整過多的車輛數
// 		把車子拆開成1台1台來處理
// 		*/
// 		size_t num_v = indv->num_vehicles();

// 		while (num_v > prob.num_vehicles())
// 		{
// 			vector <double> v_load;
// 			//cout << "vnum = " << num_v << endl;
// 			double min_load = 1e9, load = 0.0;
// 			size_t index = 0, num = 0, min_cus = 1e9, num_customers = 0; //找最少的那台的index，從0開始

// 			vector <vector<size_t>> r;
// 			vector <size_t> r_tmp;
// 			//找出最小load的那個車
// 			for (size_t i = 1; i < routes.size(); i++)
// 			{
// 				if (routes[i] == prob.depot())
// 				{
// 					r.push_back(r_tmp);
// 					//cout << "r size = " << r.size() << endl;
// 					r_tmp.clear();
// 					//cout << "r_tmp size = " << r_tmp.size() << endl;
// 					//cout << "load = " << load << endl;
// 					v_load.push_back(load);
// 					/*if (num_customers < min_cus)
// 					{
// 					min_cus = num_customers;
// 					index = num;
// 					}*/
// 					if (load < min_load)
// 					{
// 						min_load = load;
// 						index = num;
// 					}
// 					load = 0.0;
// 					num_customers = 0;
// 					num++;
// 				}
// 				else
// 				{
// 					load += n[routes[i] - 1].demand;
// 					num_customers++;
// 					r_tmp.push_back(routes[i]);
// 				}
// 			}

// 			//看每台load有多少
// 			/*cout << "load original:" << endl;
// 			for (int i = 0; i < v_load.size(); i++)
// 			{
// 			cout << "v_load[" << i << "] = " << v_load[i] << endl;
// 			}
// 			cout << endl;*/

// 			/*cout << "\noriginal :\n";
// 			for (int i = 0; i < r.size(); i++)
// 			{
// 			for (int j = 0; j < r[i].size(); j++)
// 			{
// 			cout << r[i][j] << ' ';
// 			}
// 			cout << endl;
// 			}*/
// 			//cout << "min_cus = " << min_cus << endl;
// 			//cout << "min load = " << min_load << endl;
// 			//cout << "r size = " << r.size() << endl;
// 			//cout << "v_load size = "<< v_load.size() << endl;
// 			//system("pause");
// 			//證明了不一定分割出來的一定很小
// 			bool check = false;
// 			for (size_t i = 0; i < r[index].size(); i++)
// 			{
// 				check = false;
// 				//cout << "r[index][i] = " << r[index][i] << endl;
// 				//cout << "i = " << i << endl;
// 				//cout << "r size = " << r.size() << endl;
// 				//cout << "v_load size = " << v_load.size() << endl;
// 				for (int j = index, k = index; j >= 0 || k< num; j--, k++)
// 				{
// 					//cout << "j = " << j << endl;
// 					if (j != index && j >= 0)
// 					{
// 						if (v_load[j] + n[r[index][i] - 1].demand <= prob.maxload())
// 						{
// 							r[j].push_back(r[index][i]);
// 							v_load[j] += n[r[index][i] - 1].demand;
// 							//cout << "vload[" << j << "]= " << v_load[j] << endl;
// 							check = true;
// 							break;
// 						}
// 					}
// 					//if (j < 0) j = num;
// 					if (k != index && k < num)
// 					{
// 						if (v_load[k] + n[r[index][i] - 1].demand <= prob.maxload())
// 						{
// 							//r[k].push_back(r[index][i]);
// 							r[k].insert(r[k].begin(), r[index][i]);
// 							//swap(r[k][0], r[k][r[k].size() - 1]);
// 							v_load[k] += n[r[index][i] - 1].demand;
// 							//cout << "vload[" << k << "]= " << v_load[k] << endl;
// 							check = true;
// 							break;
// 						}
// 					}
// 					//if (k == num) k = -1;
// 				}
// 				if (check == false) break;
// 			}
// 			//system("pause");
// 			//重新創造新的routes
// 			if (check)
// 			{
// 				//cout << "refine OK " << endl;
// 				routes.clear();
// 				routes.push_back(prob.depot());
// 				//cout << "0 ";
// 				for (size_t i = 0; i < num; i++)
// 				{
// 					if (i != index)
// 					{
// 						for (size_t j = 0; j < r[i].size(); j++)
// 						{
// 							routes.push_back(r[i][j]);
// 							//cout << r[i][j] << ' ';
// 						}
// 						//cout << "0 ";
// 						routes.push_back(prob.depot());
// 					}
// 				}
// 				num_v--;
// 				/*cout << "\nTrue : \n";
// 				for (int i = 0; i < routes.size(); i++) cout << routes[i] << ' ';
// 				cout << endl;*/
// 				//cout << " num v = " << num_v << endl;
// 				indv->set_num_vehicles(num_v);
// 				//system("pause");
// 			}
// 			else
// 			{
// 				//cout << "re dispatch fail" << endl;
// 				break;
// 			}
// 		}
// 	}


// 	return true;
// }
//------------------------------------------------------------------------------------------
bool CNEHRefinement::operator()(CIndividual *indv, const BProblem &prob) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();

	//BestInsert---------NEH------------------------------------------ 
	size_t head = 1, tail = 1;
	for (size_t i = 1; i < routes.size(); i++)
	{
		if (routes[i] == prob.depot())
		{
			//head 會是現在第一個不是倉庫的index,tail 是下一個倉庫的index
			tail = i;
			//先把頭換成離倉庫最近的
			double local_dis = distance[routes[i]][routes[head]];
			size_t target = head;
			for (size_t j = head + 1; j < tail; j++)
			{
				double temp_dis = distance[routes[i]][routes[j]];

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

					double total_dis = distance[prob.depot()][routes[head]];
					for (size_t h = head + 1; h <= j; h++)
					{
						if (h == tail - 1)
						{
							total_dis += distance[prob.depot()][routes[h]];
						}

						total_dis += distance[routes[h - 1]][routes[h]];
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
	size_t NEH_rate = 100;
	size_t head = 1, tail = 1;
	bool check = false;
	for (size_t i = 1; i < routes.size(); i++)
	{
		size_t NEH_now = rand() % 100;
		if (routes[i] == prob.depot())
		{
			//head 會是現在第一個不是倉庫的index,tail 是下一個倉庫的index
			tail = i;
			if (NEH_now < NEH_rate) {
				head = tail + 1;
				continue;
			}
			check = true;
			//先把頭換成離倉庫最近的
			double local_dis = distance[routes[i]][routes[head]];
			size_t target = head;
			for (size_t j = head + 1; j < tail; j++)
			{
				double temp_dis = distance[routes[i]][routes[j]];

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

					double total_dis = distance[prob.depot()][routes[head]];
					for (size_t h = head + 1; h <= j; h++)
					{
						if (h == tail - 1)
						{
							total_dis += distance[prob.depot()][routes[h]];
						}

						total_dis += distance[routes[h - 1]][routes[h]];
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
	return check;
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

					double total_dis = distance[routes[rand_head - 1]][routes[rand_head]];
					for (size_t h = rand_head + 1; h <= j; h++)
					{
						if (h == j)
						{
							total_dis += distance[routes[rand_tail + 1]][routes[h]];
						}

						total_dis += distance[routes[h - 1]][routes[h]];
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

bool CPartNEH2Object::operator()(CIndividual *indv, const BProblem &prob, const int obj1_rate, const double normal_dis, const double normal_emi) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();
	const double obj1 = (double)obj1_rate / 100, obj2 = 1 - obj1;
	const double m = 2.61, FCR_full_ = 0.390 * 10, FCR_empty_ = 0.296 * 1; //m = CER = CO2 rate
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
bool CPartNN2Object::operator()(CIndividual *indv, const BProblem &prob, const int obj1_rate, const double normal_dis, const double normal_emi) const
{
	CIndividual::TDecVec &routes = indv->routes();
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();
	const double obj1 = (double)obj1_rate / 100, obj2 = 1 - obj1;
	const double m = 2.61, FCR_full_ = 0.390 * 10, FCR_empty_ = 0.296 * 1; //m = CER = CO2 rate
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
				double local_value = obj1 * distance[routes[j - 1] - 1][routes[j] - 1] 
					+ obj2 * m *((FCR_full_ - FCR_empty_) * (prob.maxload() - n[routes[j - 1] - 1].demand) / prob.maxload())  * distance[routes[j - 1] - 1][routes[j] - 1];

				for (size_t k = j + 1; k <= rand_tail; k++)
				{
					//double temp_dis = distance[routes[j - 1] - 1][routes[k] - 1];
					double temp_value = obj1 * distance[routes[j - 1] - 1][routes[k] - 1] 
						+ obj2 * m *((FCR_full_ - FCR_empty_) * (prob.maxload() - n[routes[j - 1] - 1].demand) / prob.maxload())  * distance[routes[j - 1] - 1][routes[k] - 1];
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
	const double m = 2.61, FCR_full_ = 0.390 * 10, FCR_empty_ = 0.296 * 1; //m = CER = CO2 rate
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
				best_value = m * ((FCR_full_ - FCR_empty_) * now_load / prob.maxload())*best_dis;
			//cout << "origial route = ";
			for (size_t j = head + 1; j < tail; j++)
			{
				//cout << routes[j - 1] << ' ';
				best_dis += distance[routes[j - 1] - 1][routes[j] - 1];
				now_load += n[routes[j] - 1].demand;
				best_value += m * ((FCR_full_ - FCR_empty_) * now_load / prob.maxload()) * best_dis;
			}
			best_dis += distance[routes[tail - 1] - 1][routes[tail] - 1];
			best_value += m * FCR_empty_ * best_dis;
			best_value = obj2 * (best_value - Min_emi) / normal_emi;
			best_value += obj1 * (best_dis - Min_dis) / normal_dis;
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
						new_value = m * ((FCR_full_ - FCR_empty_) * new_load / prob.maxload())*new_dis;

					//cout << "tail - head = " << tail - head << endl;
					//cout << "new size = " << new_route.size() << endl;
					for (int c = 1; c < tail - head; c++) {

						new_dis += distance[new_route[c - 1] - 1][new_route[c] - 1];
						new_load += n[new_route[c] - 1].demand;
						new_value += m * ((FCR_full_ - FCR_empty_) * new_load / prob.maxload())*new_dis;
					}
					new_dis += distance[routes[tail] - 1][new_route[tail - head - 1] - 1];
					new_value += m * FCR_empty_ * new_dis;
					new_value = obj2 * (new_value - Min_emi) / normal_emi;
					new_value += obj1 * (new_dis - Min_dis) / normal_dis;
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
	// double NN_threshold = 1.0;
	// double NN_now = MathAux::random(0.0, 1.0);
	size_t head = 1, tail = 1;
	for (size_t i = 1; i < routes.size(); i++)
	{
		if (routes[i] == prob.depot())
		{
			tail = i;
			for (size_t j = head; j < tail - 1; j++)
			{
				size_t target = j;
				double local_dis = distance[prob.depot()][routes[j]];

				for (size_t k = j + 1; k < tail; k++)
				{
					double temp_dis = distance[prob.depot()][routes[k]];
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
	while (routes[rand_num] == prob.depot()) {// create rand num in routes
		rand_num = rand() % routes.size();
	}

	// --- search the left and right depot's indexes
	size_t left_depot = 1, right_depot = 1, now = rand_num;
	while (routes[now] != prob.depot()) {
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

	for (size_t i = left_depot + 1; i < right_depot; i++)
	{
		routes.erase(routes.begin() + rand_num);
		routes.insert(routes.begin() + i, now_cus);

		double total_dis = distance[prob.depot()][routes[left_depot + 1]];
		for (size_t h = left_depot + 2; h < right_depot; h++)
		{
			if (h == right_depot - 1)
			{
				total_dis += distance[prob.depot()][routes[h]];
			}
			total_dis += distance[routes[h - 1]][routes[h]];
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

bool CAllPointOpt::operator()(CIndividual *indv, const BProblem &prob, const int obj1_rate, 
	const double normal_dis, const double normal_emi, const double Min_dis, const double Min_emi) const
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
			size_t before_ind = 0, after_ind = 0, change_cus = routes[left_depot + 1];
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
					total_dis += distance[prob.depot() - 1][routes[right_depot - 1] - 1];
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
	// 找左右兩邊的倉庫index
	size_t left_depot = 0, right_depot = 0, now = rand_num, ori_left = rand_num, ori_right = rand_num, target = rand_num, now_cus = routes[rand_num];
	while (routes[ori_left] != prob.depot()) {
		ori_left--;
	}
	while (routes[ori_right] != prob.depot()) {
		ori_right++;
	}
	//計算原本路線扣除這點的distance
	double dis_no_target = distance[prob.depot()][routes[ori_left + 1]];
	for (size_t i = ori_left + 1; i < ori_right; i++) {
		if (i == rand_num) {
			if (i == ori_right - 1) {
				dis_no_target += distance[prob.depot()][routes[i - 1]];
			}
			continue;
		}
		if (i == ori_right - 1)
		{
			dis_no_target += distance[prob.depot()][routes[i]];
		}
		dis_no_target += distance[routes[i - 1]][routes[i]];
	}

	while (right_depot != routes.size() - 1) {
		//set好倉庫兩端(某條路線)
		left_depot = right_depot;
		right_depot++;
		//cout << "left = " << left_depot << endl;
		double load = 0.0;
		while (routes[right_depot] != prob.depot()) {
			load += n[routes[right_depot]].demand; //計算這條路線目前的load
			right_depot++;
		}
		//cout << "load = " << load << endl;
		if (load + n[routes[rand_num]].demand > prob.maxload()) { //這條路線不能多載這個貨物就不用算了
																  //cout << "continue" << endl;
			continue;
		}

		double min_dis = 1e9;

		for (size_t i = left_depot + 1; i < right_depot; i++)
		{
			routes.erase(routes.begin() + rand_num);
			routes.insert(routes.begin() + i, now_cus);

			double total_dis = distance[prob.depot()][routes[left_depot + 1]] + dis_no_target;
			for (size_t h = left_depot + 2; h < right_depot; h++)
			{
				if (h == right_depot - 1)
				{
					total_dis += distance[prob.depot()][routes[h]];
				}

				total_dis += distance[routes[h - 1]][routes[h]];
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
			double best_dis = distance[routes[head - 1]][routes[head]];
			//cout << "origial route = ";
			for (size_t j = head + 1; j < tail; j++)
			{
				//cout << routes[j - 1] << ' ';
				best_dis += distance[routes[j - 1]][routes[j]];
			}
			best_dis += distance[routes[tail - 1]][routes[tail]];
			//cout << routes[tail - 1] << endl;
			//------------------------------------
			vector<int>best_route;
			for (int j = 0; j < tail - head - 1; j++) //這裡是不是要改
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
					double new_dis = distance[routes[head - 1]][new_route[0]]; // 倉庫到第一個點的距離
																			   //cout << "tail - head = " << tail - head << endl;
																			   //cout << "new size = " << new_route.size() << endl;
					for (int c = 1; c < tail - head; c++) {
						new_dis += distance[new_route[c - 1]][new_route[c]];
					}
					new_dis += distance[routes[tail]][new_route[tail - head - 1]]; // 倉庫到最後一點的距離

					if (new_dis < best_dis) {
						best_dis = new_dis;
						best_route.clear();
						//cout << "tail - head = " << tail - head << endl;
						//cout << "new_route = ";
						for (int c = 0; c < tail - head; c++) {
							//cout << new_route[c] << ' ';
							//routes[c+head] = new_route[c];
							best_route.push_back(new_route[c]);
						}
						//cout << endl;
						//system("pause");
					}
				}
				//cout << "best route size = " << best_route.size() << endl;
				//cout << "tail - head = " << tail - head << endl;
				for (int c = 0; c < tail - head; c++) {
					//cout << new_route[c] << ' ';
					routes[c + head] = best_route[c];
				}
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
	/*把每個點都拿來插插看，插到不會再變好為止
	OnepointOPT一樣的意思? 只是call的時候跑過每個點? */
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
	double dis_no_target = distance[prob.depot()][routes[ori_left + 1]];
	for (size_t i = ori_left + 1; i < ori_right; i++) {
		if (i == rand_num) {
			if (i == ori_right - 1) {
				dis_no_target += distance[prob.depot()][routes[i - 1]];
			}
			continue;
		}
		if (i == ori_right - 1)
		{
			dis_no_target += distance[prob.depot()][routes[i]];
		}
		dis_no_target += distance[routes[i - 1]][routes[i]];
	}

	while (right_depot != routes.size() - 1) {
		//set好倉庫兩端(某條路線)
		left_depot = right_depot;
		right_depot++;
		//cout << "left = " << left_depot << endl;
		double load = 0.0;
		while (routes[right_depot] != prob.depot()) {
			load += n[routes[right_depot]].demand; //計算這條路線目前的load
			right_depot++;
		}
		//cout << "load = " << load << endl;
		if (load + n[routes[rand_num]].demand > prob.maxload()) { //這條路線不能多載這個貨物就不用算了
																  //cout << "continue" << endl;
			continue;
		}

		double min_dis = 1e9;

		for (size_t i = left_depot + 1; i < right_depot; i++)
		{
			routes.erase(routes.begin() + rand_num);
			routes.insert(routes.begin() + i, now_cus);

			double total_dis = distance[prob.depot()][routes[left_depot + 1]] + dis_no_target;
			for (size_t h = left_depot + 2; h < right_depot; h++)
			{
				if (h == right_depot - 1)
				{
					total_dis += distance[prob.depot()][routes[h]];
				}

				total_dis += distance[routes[h - 1]][routes[h]];
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

bool Initial_E::operator()(CIndividual *indv, const BProblem &prob,int s,int e) const
{
	// Initial e_up and e_down

	CIndividual::TDecVec &routes = indv->routes();
	CIndividual::TDecVec &eup = indv->e_up();
	CIndividual::TDecVec &edown = indv->e_down();
	CIndividual::TObjVec &speeds = indv->speed();

	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();
	
	edown[s] = n[prob.depot()].ready_time;
	eup[s] = n[prob.depot()].ready_time;

	for (int i = s; i < e; i++)
	{
		double driving_time = distance[routes[i]][routes[i + 1]] / speeds[i];
		edown[i + 1] = eup[i] + driving_time;
		if (i != e - 1)
		{
			eup[i + 1] = edown[i] + n[routes[i]].service_time;
		}
		else
		{
			eup[i + 1] = edown[i];
		}
	}
	return true;
}
//------------------------------------------------------------------------------------------
bool SpeedOptimalAlgorithm::operator()(CIndividual *indv, const BProblem &prob, int s, int e,int N) const
{
	cout << "s = " << s << endl;
	cout << "e = " << e << endl;
	//SOA----------- calculate the optimal speed set to the current route
	/* 
		從2012那篇的SOA模型建過來的
	*/
	// Speed Optimization Algorithm 
	CIndividual::TDecVec &routes = indv->routes();
	CIndividual::TDecVec &eup = indv->e_up();
	CIndividual::TDecVec &edown = indv->e_down();
	CIndividual::TObjVec &speeds = indv->speed();
	
	const vector<Node> & n = prob.node();
	const vector<vector<double>> & distance = prob.dis();

	/*cout << "route size = " << routes.size() << endl;
	cout << "speed size = " << speeds.size() << endl;*/
	// SOA initial parameter
	int violation = 0, p = 0;
	double D = 0.0, T = 0.0;

	// calculate D and T : total distance of this route and total service time of this route
	//cout << "calculate D T " << endl;
	for (int i = s; i<e; i++)
	{
		D += distance[routes[i]][routes[i + 1]];
		T += n[routes[i]].service_time;
	}
	cout << "T = " << T << endl;
	// find the infeasible point and adjust the optimal speed
	/*
	本身可以得到的有v,distance,route(s~e),service time(t),[a,b].
	這些只有v是可以變的
	需要計算的有 v*,e上bar,e下bar
	每一個點都有一個v*,e上bar,e下bar
	只會跟幾個有關係,是不是只需要用幾個就可以,不用開完整大小
	*/
	

	//下面這區應該不用了////////////////////////////////////////////
	/*if (routes[s] == prob.depot())
	{
		edown[s] = n[prob.depot()].ready_time;
		eup[s] = n[prob.depot()].ready_time;
	}*/

	//for (int i = s; i < e; i++) 
	//{
	//	/*cout << "distance or speed ?" << endl;
	//	cout << i << " " << i + 1 << endl;*/
	//	double driving_time = distance[routes[i]][routes[i + 1]] / speeds[i];
	//	//cout << "push back 1?" << endl;
	//	edown[i+1] = eup[eup.size() - 1] + driving_time;
	//	//cout << "push_back ?" << endl;
	//	if (i != e - 1)
	//	{
	//		eup[i+1] = edown[edown.size() - 1] + n[i].service_time;
	//	}
	//	else
	//	{
	//		eup[i+1] = edown[edown.size() - 1];
	//	}
	//}
	////////////////////////////////////////////////////////////////

	// SOA process 
	//cout << "SOA process" << endl;
	for (int i = s + 1; i <= e; i++)
	{
		cout << "i = " << i << endl;
		// 1.adjust the infeasible 
		//cout << "1" << endl;
		/*cout << "i = " << i << endl;
		cout << "speeds size = " << speeds.size() << endl;
		cout << "down size = " << e_down.size() << endl;
		cout << "n size = " << n.size() << endl;
		cout << "up size = " << e_up.size() << endl;*/
		
		if (edown[i] < n[routes[i]].ready_time) // too early 
		{
			cout << "s1 ";
			//speeds[i - 1] = D / (eup[i] - n[routes[i]].ready_time - T); // paper version 
			speeds[i - 1] = distance[routes[i - 1]][routes[i]] / (n[routes[i]].ready_time - eup[i - 1]);// my realize version
		}
		else if(edown[i] > n[routes[i]].due_time) // too late
		{
			cout << "s2 ";
			//speeds[i - 1] = D / (eup[i] - edown[i] - T); // paper version
			speeds[i - 1] = distance[routes[i - 1]][routes[i]] / (n[routes[i]].due_time - eup[i - 1]); // my realize version
		}
		cout << "step 1 speed[i-1] = " << speeds[i - 1] << endl;
		// 2.set the v* 
		/*
		minimizes fuel consumption costs and wage of driver is :
		v* = (kNV/2Br + fd/(2B*lamda*r*fc))^(1/3)
		kNV = 0.2 x 33 x 5 = 33
		B = 0.5 x 0.7 x 1.2041 x 3.912 = 1.64865372
		r = 0.0027778
		2Br = 0.009159260606832
		kNv/2Br = 3602.910913505934
		fd = driver wage per ($/second) = 0.0022
		2B = 3.29730744
		lamda = 0.00003083
		r = gama = 1/(1000 x 0.4 x 0.9) = 0.0027778
		fc = Fuel and CO2 emissions cost per liter ($) = 1.4
		(2B*lamda*r*fc) = 0.000000395332006312082784
		fd/(2B*lamda*r*fc) = 5564.9427945969979467370236720478
		kNV/2Br + fd/(2B*lamda*r*fc) = 9167.8537081029319467270236720475
		v* = 20.9294 (m/s) = 75.34584 (km/h)
		先不用這個.. ?

		minimizes fuel consumption costs is :
		v* = (kNV/2Br)^(1/3)
		k = 0.2(引擎磨擦係數), N = 33(引擎轉速)(轉/秒),V = 5(發動機排量)
		kNV = 0.2 x 33 x 5 = 33
		B = 0.5 x 0.7 x 1.2041 x 3.912 = 1.64865372
		r = 0.0027778
		2Br = 0.009159260606832
		kNv/2Br = 3602.910913505934
		v* = 15.3303 (m/s) = 55.18908 (km/h)
		*/
		double v_star = 0.0;
		//v_star = 15.3303; //minimizes fuel consumption costs,(about 55 km/h)
	    v_star = 20.9294; //minimizes fuel consumption costs and wage of driver(about 90km,h) 

		// 3.if arrive too early 
		//cout << "3" << endl;
		/*
			用e_up算會太早到 i 點 且 用e_down看i點到達時間會>=最早可以離開的時間
			這樣的意思是e_up被調過... 可是被調過的e_up應該要是被調整成的，不會是錯的?
		*/
		if (eup[i - 1] + distance[routes[i - 1]][routes[i]] / speeds[i - 1] < n[routes[i]].ready_time &&
			edown[i] >= n[routes[i]].ready_time + n[routes[i]].service_time &&
			i != n.size() - 1)
		{
			cout << "3.1 - speed[i-1] =" << speeds[i - 1] << endl;
			//speeds[i - 1] = distance[routes[i - 1]][routes[i]] / (n[routes[i]].ready_time - edown[i - 1]); // paper version
			speeds[i - 1] = distance[routes[i - 1]][routes[i]] / (n[routes[i]].ready_time - edown[i - 1] - n[routes[i-1]].service_time); //  my realize version
		}
		/*
			前面意思一樣看eup[i-1]太早到,但是且eup[i] >= 最晚服務時間(太晚到)
			應該是eup 被調整過才會長成這樣
		*/
		else if (eup[i - 1] + distance[routes[i - 1]][routes[i]] / speeds[i - 1] < n[routes[i]].ready_time &&
			     eup[i] >= n[routes[i]].due_time + n[routes[i]].service_time &&
			     i != n.size() - 1)
		{
			cout << "3.2 - speed[i-1] = " << speeds[i - 1] << endl;
			//speeds[i - 1] = distance[routes[i - 1]][routes[i]] / (n[routes[i]].due_time - edown[i - 1] ); // paper version
			speeds[i - 1] = distance[routes[i - 1]][routes[i]] / (n[routes[i]].due_time - edown[i - 1] - n[routes[i - 1]].service_time);//  my realize version
		}
		// 4.last point in route ? 
		//cout << "4" << endl;
		if (i == N - 1 && eup[i] != edown[i])
		{
			//speeds[i - 1] = distance[routes[i - 1]][routes[i]] / (n[routes[i]].ready_time - eup[i - 1]);//paper version
			cout << "step 4 speed = " << speeds[i - 1] << endl;
			if (edown[i] > n[routes[i]].due_time)
			{
				speeds[i - 1] = distance[routes[i - 1]][routes[i]] / (n[routes[i]].due_time - eup[i - 1]); // my version
			}
			
		}
		
		// 5.adjust the speed to meet the time window
		//cout << "5" << endl;
		if (v_star < distance[routes[i - 1]][routes[i]] / (n[routes[i]].due_time - n[routes[i - 1]].ready_time - n[routes[i - 1]].service_time))
		{
			v_star = distance[routes[i - 1]][routes[i]] / (n[routes[i]].due_time - n[routes[i - 1]].ready_time - n[routes[i - 1]].service_time);
			cout << "step 5 update v* = " << v_star << endl;
		}

		// 6.compare the v and v*
		//cout << "6" << endl;
		if (v_star > speeds[i - 1])
		{
			speeds[i - 1] = v_star;
			cout << "step 6 (change to v*)speed[i-1] = " << speeds[i - 1] << endl;
		}
		cout << "final speed = " << speeds[i - 1] << endl;
		// 7.update e_down
		cout << "distance = " << distance[routes[i - 1]][routes[i]] << endl;
		cout << "eup[i-1](leave) = " << eup[i - 1] << endl;
		cout << "edown = eup + dis / speed" << endl;
		edown[i] = eup[i - 1] + distance[routes[i - 1]][routes[i]] / speeds[i - 1];
		

		// 8.update e_up
		if (i != e)
		{
			eup[i] = edown[i] + n[routes[i]].service_time;
			// cout << "service time = " << n[routes[i]].service_time << endl;
		}
		// 9.set the gi 
		/*
			前者是太晚到,後者是太早到
		*/
		double gi = max(edown[i] - n[routes[i]].due_time, n[routes[i]].ready_time + n[routes[i]].service_time - eup[i]);
		cout << "final edown[i](arrival) = " << edown[i] << endl;
		cout << "due time = " << n[routes[i]].due_time << endl;
		cout << "(too late)e_down[i - 1] - n[routes[i]].due_time = " << edown[i] - n[routes[i]].due_time << endl;
		cout << "e_up[i](leave) = " << eup[i] << endl;
		cout << "ready time = " << n[routes[i]].ready_time << endl;
		cout << "service time = " << n[routes[i]].service_time << endl;
		cout << "(too early)n[routes[i]].ready_time + n[routes[i]].service_time - eup[i] = " << n[routes[i]].ready_time + n[routes[i]].service_time - eup[i] << endl;
		cout << "gi = " << gi << endl;
		if (gi < 0.0) gi = 0.0;
		

		// 10.set the violation
		if (gi > violation)
		{
			cout << "violation " << i << endl; getchar();
			violation = gi;
			p = i;
		}
		// getchar();
	}

	cout << "\neup(leave) board :" << endl;
	for (int i = 0; i < eup.size(); i++)
	{
		cout << eup[i] << " " << endl; 
	}
	cout << endl << endl;

	cout << "\nedown(arrive) board :" << endl;
	for (int i = 0; i < edown.size(); i++)
	{
		cout << edown[i] << " " << endl;
	}
	cout << endl << endl;
	//cout << "p = " << p << endl;
	// recursive SOA
	// cout << "recur" << endl;
	if (violation > 0 && edown[p] > n[routes[p]].due_time)
	{
		cout << "too late" << endl;
		eup[p] = n[routes[p]].due_time + n[routes[p]].service_time;
		operator()(indv, prob, s, p, N);
		operator()(indv, prob, p, e, N);
	}
	if (violation > 0 && eup[p] < n[routes[p]].ready_time + n[routes[p]].service_time)
	{
		cout << "too early" << endl;  getchar();
		eup[p] = n[routes[p]].ready_time + n[routes[p]].service_time;
		operator()(indv, prob, s, p, N);
		operator()(indv, prob, p, e, N);
	}
	cout << "OK" << endl; getchar();

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
		if (routes[i] == prob.depot()) {
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