#include "problem_self.h"
#include "alg_individual.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <algorithm>

using namespace std;

CProblemSelf::CProblemSelf(std::size_t num_vars, std::size_t num_objs, const std::string name) :
	BProblem(name),
	num_vars_(num_vars),
	num_objs_(num_objs)
{
	// define the domain of variables here

	ifstream ifile("Experiments//" + name_ + ".txt");
	if (!ifile)
	{
		cout << "Not find " << name_ << endl; getchar();
		return;
	}
	else cout << "Read the file -- " << name_ << endl;

	ifile >> num_customers_ >> curb_weight_ >> maxload_;
	ifile >> lowest_speed_ >> highest_speed_;
	num_node_ = num_customers_ + 1;
	//distance 
	for (int i = 0; i < num_node_; i++)
	{
		vector <double> dis_one_array;
		for (int j = 0; j < num_node_;j++)
		{
			double tmp_dis;
			ifile >> tmp_dis;
			dis_one_array.push_back(tmp_dis);
		}
		distance_.push_back(dis_one_array);
	}

	//print distance board
	/*for (int i = 0; i < distance_.size(); i++)
	{
		for (int j = 0; j < distance_[i].size(); j++)
		{
			cout << distance_[i][j] << " ";
		}
		cout << endl;
	}*/

	for (int i = 0; i < num_node_; i++)
	{
		Node tmp_node;
		ifile >> tmp_node.number >> tmp_node.name >> tmp_node.demand >> tmp_node.ready_time >> tmp_node.due_time >> tmp_node.service_time;
		if (tmp_node.service_time == 0)
		{
			depot_section_ = tmp_node.number;
		}
		node_.push_back(tmp_node);
	}

	for (int i = 0; i < num_node_; i++)
	{
		cout << node_[i].number << " " << node_[i].name << " " << node_[i].demand << " " << node_[i].ready_time << " " << node_[i].due_time << " " << node_[i].service_time << endl;
	}

	//num_vars_ = num_node_; //我不知道這有什麼意思..

	//feasible_dis_ = 5e3; //還不確定
	cout << "Problem set ------- OK" << endl;
	getchar();

}
// -----------------------------------------------------------
bool CProblemSelf::EvaluateDpCar(CIndividual *indv) const
{
	const BProblem &prob = *this;

	const CIndividual::TDecVec &x = indv->vars();
	const vector<Node> & n = prob.node();

	bool success = false;
	while (!success)
	{
		//there is no depot  -- initial dis_board = 0.0
		vector <vector<double>> dis_board;
		for (size_t i = 0; i < prob.num_node() - 1; i++)
		{
			dis_board.push_back(vector<double>());
			for (size_t j = 0; j < prob.num_node() - 1; j++)
			{
				dis_board[i].push_back(0.0);
			}
		}

		//1~num_node-1 , no depot  --- calculate dis_board
		/************************************************************************************
		dis_board 只有客戶點，所以0~num_node-1，dis_board[0][0]指的昰 (倉庫->客戶第一個->倉庫) 的距離
		超越載重就會用 9999999 來破壞解
		dis_board[i][j]的意思就是 (倉庫 -> i客戶 -> ...(依序) -> j客戶 -> 倉庫) 的距離
		*************************************************************************************/
		for (size_t i = 0; i<prob.num_node() - 1; i++)
		{
			bool overload = false;
			double dis_temp = 2 * distance_[x[i + 1] - 1][prob.depot() - 1], //Ex. 0-1-0,0-2-0
				load = n[x[i + 1] - 1].demand;
			//cout << "demand 1 " << n[x[i + 1] - 1].demand << endl;
			dis_board[i][i] = dis_temp;
			//cout << "dis_board[" << i << "][" << i << "] = " << dis_temp << endl;
			for (size_t j = i + 1; j < prob.num_node() - 1; j++)
			{
				if (overload)
				{
					dis_board[i][j] = 9999999;
					continue;
				}

				load += n[x[j + 1] - 1].demand;
				//cout << "demand 2 " << n[x[j+1] - 1].demand << endl;
				//system("pause");

				if (load > prob.maxload())
				{
					//cout << "overload !" << endl;
					//dis_temp = 999999;//old
					dis_temp = 9999999;
					overload = true;
				}
				dis_temp += distance_[x[j + 1] - 1][x[j] - 1]; //0-1-0+1-2
															   //cout << "+" << sqrt(pow(n[x[j + 1] - 1].x - n[x[j] - 1].x, 2) + pow(n[x[j + 1] - 1].y - n[x[j] - 1].y, 2)) << endl;
				dis_temp -= distance_[x[j] - 1][prob.depot() - 1];//0-1+1-2 = 0-1-2
																  //cout << "-" << sqrt(pow(n[x[j] - 1].x - n[prob.depot() - 1].x, 2) + pow(n[x[j] - 1].y - n[prob.depot() - 1].y, 2)) << endl;
				dis_temp += distance_[x[j + 1] - 1][prob.depot() - 1]; //0-1-2-0
																	   //cout << "+" << sqrt(pow(n[x[j + 1] - 1].x - n[prob.depot() - 1].x, 2) + pow(n[x[j + 1] - 1].y - n[prob.depot() - 1].y, 2)) << endl;
				dis_board[i][j] = dis_temp;
				//cout << "dis_board[" << i << "][" << j << "] = " << dis_board[i][j]<< endl;
			}
		}

		//DP calculate good cut point
		/*
		dp[i][j] 的意思是到點i用j+1台車的最短距離，i=0是倉庫，dp[0][j] = 0
		j表示了貨車數量，所以不需要car來存幾台車了
		*/
		vector<vector<double>> dp; //2維DP 
								   //initial dp  = 0.0---------------------------------------------
		for (size_t i = 0; i < prob.num_node(); i++)
		{
			dp.push_back(vector<double>());
			for (size_t j = 0; j < prob.num_vehicles() + 2; j++)
			{
				if (j == 0) {
					if (i == 0) {
						dp[i].push_back(0.0);
					}
					dp[i].push_back(dis_board[0][i - 1]);
					//cout << "dp[" << i << "] = " << dp[i][0] << endl;
				}
				else dp[i].push_back(0.0);
			}
		}


		//--------------------------------------------------------
		//cout << "dp 初始化完成" << endl;
		vector<vector<size_t>> cut_point;
		for (size_t i = 0; i < prob.num_node(); i++)
		{
			cut_point.push_back(vector<size_t>());
		}
		//cout << "cut_point 初始化完成" << endl;
		//cout << "prob.num_vehicles = " << prob.num_vehicles() << endl;
		//cout << "倉庫 = " << prob.depot() << endl;
		/*cout << "序列 = ";
		for (int i = 0; i < x.size(); i++)
		{
		cout << x[i] << ' ';
		}
		cout << endl;*/
		for (size_t c = 1; c < prob.num_vehicles(); c += 1) //c+1代表可多用1台車
		{
			//cout << "現在可用" << c + 1 << "台車" << endl;
			for (size_t i = 1; i < prob.num_node(); i += 1) //num_node is num of points
			{
				//dp[i] = dp[i-1] + dis_board[i - 1][i - 1];
				double min_dis = dis_board[0][i - 1]; //第一次是 = 0-1-0(原本有+dp[0]可是dp[0]就always是0)
													  //cout << "min dis_board[0][" << i - 1 << "]=" << dis_board[0][i - 1] << endl;
													  /*cout << " = " << "0-" << i - 1 << "-0" << endl;*/
				size_t cut = 0;
				/*
				找找看有沒有新切法對於dp[i] 假如 i = 4
				會試過 dp[0] + 0-1-2-3-4-0
				dp[1] + 0-2-3-4-0
				dp[2] + 0-3-4-0
				dp[3] + 0-4-0
				除了dp[0]是最短以外，其他路線最短把num car都會 = car[j]+1並且cut = j，新切點要切在j位置
				*/
				for (size_t j = 1; j < i; j++)
				{
					//cout << "j = " << j << endl;
					//cout << "ca dis = " << dp[j] + dis_board[j][i - 1] << endl;

					/*
					如果dp[j]+ 0->j->i-1->0 < min_dis 就改成 min_dis
					cut 變 j
					車輛數 = car[j] + 1
					*/
					/*cout << "dp[" << j << "][" << c-1 << "] = " << dp[j][c-1] << endl;
					cout << "disboard[" << j << "][" << i - 1 << "] = " << dis_board[j][i - 1] << endl;
					cout << "min_dis = " << min_dis << endl;*/
					if (dp[j][c - 1] + dis_board[j][i - 1] < min_dis)//&& (car[j] + 1 <= prob.num_vehicles())) 
					{
						//cout << "more car is good !" << endl;
						min_dis = dp[j][c - 1] + dis_board[j][i - 1];
						/*cout << "dp[" << j << "][" << c - 1 << "]= " << dp[j][c - 1] << endl;
						cout << "dis_board[" << j << "][" << i - 1 << "]=" << dis_board[j][i - 1] << endl;*/

						cut = j;
						//cut is the end of one car 
					}
				}
				//dp[i] = dp[i - 1] + dis_board[i - 1][i - 1];
				dp[i][c] = min_dis;
				//cout << "so dp[" << i << "][" << c << "] = " << dp[i][c] << endl;
				cut_point[i].push_back(cut);

				/*cout << "cut point " << i << " : ";
				for (int k = 0; k < cut_point[i].size(); k++)
				{
				cout << cut_point[i][k] << ' ';
				}
				cout << endl;*/
				//cout << "dp[" << i << "] = " << min_dis << endl;
			}
		}
		vector <size_t> new_cut_point;
		size_t now_index = prob.num_node() - 1, car = prob.num_vehicles() - 2;
		while (1)
		{
			if (cut_point[now_index][car] == 0)break;
			//cout << "cut_point[" << now_index << "][" << car << "] = " << cut_point[now_index][car] <<  endl;
			new_cut_point.push_back(cut_point[now_index][car]);
			now_index = cut_point[now_index][car];
			if (car == 0)break;
			car--;
		}

		indv->set_num_vehicles(prob.num_vehicles());
		//cout << "dp[prob.num_vehicles() - 1] = " << dp[prob.num_node() - 1][prob.num_vehicles() - 1] << endl;
		//if (dp[prob.num_node() - 1][prob.num_vehicles()-1] >= 999998)
		//{
		//	if (dp[prob.num_node() - 1][prob.num_vehicles()] >= 999998) {
		//		indv->set_num_vehicles(prob.num_vehicles() + 2);
		//	}
		//	else {
		//		indv->set_num_vehicles(prob.num_vehicles() + 1);
		//	}
		//}//-------------------------------------------------------------
		success = true;

		/*cout << "cut point : ";
		for (int i = 0; i < new_cut_point.size(); i++)
		{
		cout << new_cut_point[i] << ' ';
		}
		cout << endl;*/

		//插入倉庫到正確位置
		CIndividual::TDecVec &routes = indv->routes();
		routes = x;
		/*for (size_t i = 0; i < cut_point[prob.num_node() - 1].size(); i++)
		{
		routes.insert(routes.begin() + cut_point[prob.num_node() - 1][i] + i + 1, prob.depot());
		}*/
		for (size_t i = 0; i < new_cut_point.size(); i++)
		{
			routes.insert(routes.begin() + new_cut_point[i] + 1, prob.depot());
		}
		routes.push_back(prob.depot());
		/*cout << "route size = " << routes.size() << endl;

		cout << "finish chromesome = ";
		for (size_t i = 0; i < routes.size(); i++)
		{
		cout << routes[i] << ' ';
		}
		cout << endl;*/

	}
	return this->EvaluateOldEncoding(indv);
}
// -----------------------------------------------------------

bool CProblemSelf::Dp2Object(CIndividual *indv, int obj1_rate) const
{
	const BProblem &prob = *this;
	const double obj1 = (double)obj1_rate / 100, obj2 = 1 - obj1;
	const int weight_punishment = 1e8;

	const CIndividual::TDecVec &x = indv->vars();
	const vector<Node> & n = prob.node();

	bool success = false;
	while (!success)
	{
		//there is no depot  -- initial dis_board = 0.0
		vector <vector<double>> dis_board;
		for (size_t i = 0; i < prob.num_node() - 1; i++)
		{
			dis_board.push_back(vector<double>());
			for (size_t j = 0; j < prob.num_node() - 1; j++)
			{
				dis_board[i].push_back(0.0);
			}
		}
		//1~num_node-1 , no depot  --- calculate dis_board
		/************************************************************************************
		dis_board 只有客戶點，所以0~num_node-1，dis_board[0][0]指的昰 (倉庫->客戶第一個->倉庫) 的距離
		超越載重就會用 weight_punishment = 1e8 來破壞解
		dis_board[i][j]的意思就是 (倉庫 -> i客戶 -> ...(依序) -> j客戶 -> 倉庫) 的距離
		*************************************************************************************/
		for (size_t i = 0; i<prob.num_node() - 1; i++)
		{
			bool overload = false;
			double dis_temp = 2 * distance_[x[i + 1] - 1][prob.depot() - 1], //Ex. 0-1-0,0-2-0
				load = n[x[i + 1] - 1].demand;
			//cout << "demand 1 " << n[x[i + 1] - 1].demand << endl;
			dis_board[i][i] = dis_temp;
			//cout << "dis_board[" << i << "][" << i << "] = " << dis_temp << endl;
			for (size_t j = i + 1; j < prob.num_node() - 1; j++)
			{
				if (overload)
				{
					dis_board[i][j] = weight_punishment;
					continue;
				}
				load += n[x[j + 1] - 1].demand;
				if (load > prob.maxload())
				{
					dis_temp = weight_punishment;
					overload = true;
				}
				dis_temp += distance_[x[j + 1] - 1][x[j] - 1]; //0-1-0+1-2
															   //cout << "+" << sqrt(pow(n[x[j + 1] - 1].x - n[x[j] - 1].x, 2) + pow(n[x[j + 1] - 1].y - n[x[j] - 1].y, 2)) << endl;
				dis_temp -= distance_[x[j] - 1][prob.depot() - 1];//0-1+1-2 = 0-1-2
																  //cout << "-" << sqrt(pow(n[x[j] - 1].x - n[prob.depot() - 1].x, 2) + pow(n[x[j] - 1].y - n[prob.depot() - 1].y, 2)) << endl;
				dis_temp += distance_[x[j + 1] - 1][prob.depot() - 1]; //0-1-2-0
																	   //cout << "+" << sqrt(pow(n[x[j + 1] - 1].x - n[prob.depot() - 1].x, 2) + pow(n[x[j + 1] - 1].y - n[prob.depot() - 1].y, 2)) << endl;
				dis_board[i][j] = dis_temp;
			}
		}
		//計算emission board
		//initial emission board = 0.0
		vector<vector<double>> emission_board;
		for (size_t i = 0; i < prob.num_node(); i++)
		{
			emission_board.push_back(vector<double>());
			for (size_t j = 0; j < prob.num_node() - 1; j++)
			{
				emission_board[i].push_back(0.0);
			}
		}
		//caculate emission 
		for (size_t i = 0; i<prob.num_node() - 1; i++)
		{
			double emi_temp = m_ * ((FCR_full_ - FCR_empty_) * n[x[i + 1] - 1].demand / maxload_ + FCR_empty_) * distance_[x[i + 1] - 1][prob.depot() - 1],
				load_now = n[x[i + 1] - 1].demand,
				dis_temp = distance_[x[i + 1] - 1][prob.depot() - 1];
			//dis_board[i][i] = dis_temp;
			emission_board[i][i] = emi_temp;
			//cout << "dis_board[" << i << "][" << i << "] = " << dis_temp << endl;
			for (size_t j = i + 1; j < prob.num_node() - 1; j++)
			{
				if (dis_board[i][j] == weight_punishment)
				{
					emission_board[i][j] = weight_punishment;
					continue;
				}
				load_now += n[x[j + 1] - 1].demand;
				
				dis_temp += distance_[x[j + 1] - 1][x[j] - 1]; //0-1-0+1-2
															   //cout << "+" << sqrt(pow(n[x[j + 1] - 1].x - n[x[j] - 1].x, 2) + pow(n[x[j + 1] - 1].y - n[x[j] - 1].y, 2)) << endl;
				emi_temp = m_ * ((FCR_full_ - FCR_empty_) * load_now / maxload_ + FCR_empty_) * dis_temp;
				emission_board[i][j] = emi_temp;
			}
		}

		//DP calculate good cut point
		/*
		dp[i][j] 的意思是到點i用j+1台車的最短距離，i=0是倉庫，dp[0][j] = 0
		j表示了貨車數量，所以不需要car來存幾台車了
		*/
		vector<vector<double>> dp; //2維DP 
								   //initial dp  = 0.0---------------------------------------------
		for (size_t i = 0; i < prob.num_node(); i++)
		{
			dp.push_back(vector<double>());
			for (size_t j = 0; j < prob.num_vehicles() + 2; j++)
			{
				if (j == 0) {
					if (i == 0) {
						dp[i].push_back(0.0);
					}
					else dp[i].push_back(dis_board[0][i - 1]);
					
				}
				else dp[i].push_back(0.0);
			}
		}
		//--------------------------------------------------------
		//cout << "dp 初始化完成" << endl;
		vector<vector<size_t>> cut_point;
		for (size_t i = 0; i < prob.num_node(); i++)
		{
			cut_point.push_back(vector<size_t>());
		}
		//改成while(!feasible ||ｔ<= k)
		size_t car = 1; 
		bool feasible = false;
		while (!feasible || car <= prob.num_vehicles()-1)
		{
			//car = 1 其實在算2台車
			feasible = false;
			//cout << "現在用" << car << "台車" << endl;
			for (size_t i = 1; i < prob.num_node(); i += 1) //num_node is num of points
			{
				//dp[i] = dp[i-1] + dis_board[i - 1][i - 1];
				double min_value = obj1 * dis_board[0][i - 1] + obj2 * emission_board[0][i - 1]; //第一次是 = 0-1-0(原本有+dp[0]可是dp[0]就always是0)
				//double min_value = dis_board[0][i - 1] ; //第一次是 = 0-1-0(原本有+dp[0]可是dp[0]就always是0)

				if (dis_board[0][i - 1] >= weight_punishment)
				{
					min_value = weight_punishment;
				}
				//cout << "ori min value = " << min_value << endl;;
				size_t cut = 0;

				for (size_t j = 1; j < i; j++)
				{
					//printf("dis_board[%d][%d] = %.2f\n", j, i - 1, dis_board[j][i - 1]);
					if (dis_board[j][i - 1] >= weight_punishment*0.1) {
						/*cout << "punish !!" << endl;
						printf("%f > %d ?\n", dis_board[j][i - 1], weight_punishment);*/
						continue;
					}
					if (dp[j][car - 1] >= weight_punishment*0.1)
					{
						continue;
					}
					
					if (dp[j][car - 1] + obj1 * dis_board[j][i - 1] + obj2 *emission_board[j][i - 1] < min_value)//&& (car[j] + 1 <= prob.num_vehicles())) 
					//if (dp[j][car - 1] + dis_board[j][i - 1]  < min_value)//&& (car[j] + 1 <= prob.num_vehicles())) 
					{
						min_value = dp[j][car - 1] + dis_board[j][i - 1] + obj2 * emission_board[j][i - 1];
						//min_value = dp[j][car - 1] + dis_board[j][i - 1] ;
						/*printf("dp[%d][%d] = %.2f\n", j, car-1, dp[j][car - 1]);
						printf("dis_board[%d][%d] = %.2f\n", j, i - 1, dis_board[j][i - 1]);
						cout << "in min_value = " << min_value << endl;*/
						cut = j;
						//cut is the end of one car 
					}
				}
				if (min_value >= weight_punishment)
				{
					dp[i][car] = weight_punishment;	
				}
				else
				{
					dp[i][car] = min_value;
				}
				/*cout << "min v = " << min_value << endl;
				printf("cut dp[%d][%d] = %.2f\n", cut, car-1, dp[cut][car-1]);
				printf("dp[%d][%d] = %.2f\n", i, car, min_value);*/
				if (min_value > 2e5 && min_value < weight_punishment) getchar();
				//cout << "cut = " << cut << endl;
				//getchar();
				cut_point[i].push_back(cut);
				/*if (car == prob.num_vehicles()-1)
				{
					for (int k = 0; k < cut_point[i].size(); k++) {
					printf("cut_point[%d][%d] = %d\n", i,k, cut_point[i][k]);
					}
				}*/
				
				
			}
			//getchar();
			if (dp[prob.num_node() - 1][car]< weight_punishment)
			{
				//printf("dp[%d][%d] = %lf\n", prob.num_node() - 1, car, dp[prob.num_node() - 1][car]);
				if (car >= prob.num_vehicles()-1) {
					feasible = true;
					break;
				}
			}
			car++;
		}
		//cout << "car = " << car << endl;
		indv->set_num_vehicles(car+1);
		car -= 1;//car 從0開始就是用1台車所以要-1
		vector <size_t> new_cut_point;
		//size_t now_index = prob.num_node() - 1, car = prob.num_vehicles() - 2;//0607改
		size_t now_index = prob.num_node() - 1;

		while (1)
		{
			/*printf("now_index = %d\n", now_index);
			printf("car = %d\n", car);
			printf("cut_point[%d][%d] = %d\n", now_index, car, cut_point[now_index][car]);*/
			if (cut_point[now_index][car] == 0) {
				//getchar();
				break;
			}
			new_cut_point.push_back(cut_point[now_index][car]);
			now_index = cut_point[now_index][car];
			if (car == 0) {
				//getchar();
				break;
			}
			car--;
		}
		//cout << "new cut size = " << new_cut_point.size() << endl;
		success = true;


		//插入倉庫到正確位置
		CIndividual::TDecVec &routes = indv->routes();
		routes = x;

		for (size_t i = 0; i < new_cut_point.size(); i++)
		{
			routes.insert(routes.begin() + new_cut_point[i] + 1, prob.depot());
		}
		routes.push_back(prob.depot());

	}
	return this->EvaluateOldEncoding(indv);
}
// -----------------------------------------------------------

bool CProblemSelf::EvaluateOldEncoding(CIndividual *indv) const
{
	const CIndividual::TDecVec &routes = indv->routes();
	CIndividual::TObjVec &f = indv->objs();
	const double infeasible_value = 1e8;

	// --- Check if solution is valid.
	vector <bool> check_gene;
	check_gene.resize(routes.size(), false);
	bool continous_depot = false;
	double load_check = 0.0;
	//cout << "\nEva :\n";
	for (size_t i = 0; i<routes.size(); i++)
	{
		//cout << routes[i] << ' ';
		if (routes[i] == depot()) {
			load_check = 0;
			if (continous_depot)
			{
				f[0] = infeasible_value;
				f[1] = infeasible_value;
				cout << "routes = " << routes[i - 1] << ' ' << routes[i] << endl;
				cout << "continous Invalid !" << endl;
				system("pause");
				return false;
			}
			continous_depot = true;
		}
		else if (check_gene[routes[i]])
		{
			f[0] = infeasible_value;
			f[1] = infeasible_value;
			//cout << "rout i = " << routes[i] << endl;
			cout << "same customers Invalid !" << endl;
			getchar();
			return false;
		}
		else
		{
			load_check += node_[routes[i] - 1].demand;
			//cout << "load = " << load_check << endl;
			if (load_check > maxload_)
			{
				f[0] = infeasible_value;
				f[1] = infeasible_value;
				cout << "load Invalid !" << endl;
				getchar();
				return false;
			}
			check_gene[routes[i]] = true;
			continous_depot = false;
		}
	}

	// You can define your own problem here.

	//distance
	double next_posx, next_posy,
		now_pos = depot_section_ - 1, next_pos;

	bool cycle = false;

	vector <double> v_dis;
	double dis;

	/*cout << "now dis = " << f[0] << endl;
	cout << "indv v = " << indv->num_vehicles() << endl;
	cout << "this v = " << this->num_vehicles() << endl;
	cout << "max XXX = " << max(indv->num_vehicles() - this->num_vehicles(), static_cast<size_t>(0)) << endl;
	cout << "static cast = " << static_cast<size_t>(0) << endl;
	cout << "-- = " << (int)indv->num_vehicles() - (int)this->num_vehicles() << endl;*/
	int limit_vehicles = (int)indv->num_vehicles() - (int)this->num_vehicles();
	//f[0] = max((int)indv->num_vehicles() - (int)this->num_vehicles(), static_cast<size_t>(0))*5e3;
	f[0] = max(limit_vehicles, static_cast<int>(0))*5e3;

	//object 1 (distance) : total distance
	for (size_t i = 1; i<routes.size(); i++)
	{
		next_pos = routes[i] - 1;
		dis = distance_[now_pos][next_pos];
		v_dis.push_back(dis);
		f[0] += dis;

		now_pos = next_pos;
	}
	//cout << "car = " << indv->num_vehicles() << endl;
	//cout << "total distance = " << f[0] << endl;
	//getchar();
	//f[0] = x[0];

	///object 2 (Co2 Emission): load * distance
	/*
	CO2 emission = m x fuel consumption
	fuel consumption = (a x 10^-3 x load + b ) x distance
	m = 2.68 (油耗=>CO2的係數)
	a = 6.208 x 10^-3
	b = 0.2125  (空車重)
	*/
	//f[1] = max(indv->num_vehicles() - this->num_vehicles(), static_cast<size_t>(0))*1e5;
	f[1] = max(limit_vehicles, static_cast<int>(0))*1e5;
	//cout << "before f[1] = " << f[1] << endl;
	size_t next_index = 0, now_index = 0;
	double load = 0;
	for (size_t i = 0; i<routes.size(); i++)
	{
		load += node()[routes[i] - 1].demand;
		if (routes[i] == depot() && i != 0)
		{
			next_index = i;
			for (size_t j = now_index; j<next_index; j++)
			{
				f[1] += ((FCR_full_ - FCR_empty_) * load / maxload_ + FCR_empty_) * v_dis[j]; //[ZHANG]
				//f[1] += (a_ * load / maxload_ + b_) * v_dis[j];//JEMEI
				//cout << "emission = " << f[1] << endl;
				//getchar();
				load -= node()[routes[j + 1] - 1].demand;
			}
			now_index = next_index;
			load = 0;
		}
	}
	f[1] *= m_; // CO2 emission = m x fuel consumption
	  		//cout << "after f[1] = " << f[1] << endl;
				//超出車輛數的判斷
				//cout << endl;
				//getchar();
	if (indv->num_vehicles() > this->num_vehicles())
	{
		//cout << "vehicle overload = " << indv->num_vehicles() << endl;
		//f[0] = max(indv->num_vehicles() - this->num_vehicles(), static_cast<size_t>(0))*5e3;
		//f[1] = max(indv->num_vehicles() - this->num_vehicles(), static_cast<size_t>(0))*1e5;
		return false;
	}
	//else
	//cout << "vehicle OK" << endl;


	return true;
}
