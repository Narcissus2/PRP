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
	// 單位轉換：把KM/H 轉換成 M/S
	lowest_speed_ = lowest_speed_ * 1000 / 3600;
	highest_speed_ = highest_speed_ * 1000 / 3600;
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

	// time window
	for (int i = 0; i < num_node_; i++)
	{
		Node tmp_node;
		ifile >> tmp_node.number >> tmp_node.name >> tmp_node.demand >> tmp_node.ready_time >> tmp_node.due_time >> tmp_node.service_time;
		if (tmp_node.service_time == 0)
		{
			//cout << "depot = " << depot_section_ << endl;
			depot_section_ = tmp_node.number;
		}
		node_.push_back(tmp_node);
	}
	start_time_ = node_[depot_section_].ready_time;
	cout << "start time = " << start_time_ << endl;
	// ----- output the map information -----
	/*for (int i = 0; i < num_node_; i++)
	{
		cout << node_[i].number << " " << node_[i].name << " " << node_[i].demand << " " << node_[i].ready_time << " " << node_[i].due_time << " " << node_[i].service_time << endl;
	}*/

	//num_vars_ = num_node_; //我不知道這有什麼意思..
	// fc = fuel cost 的係數, fd = driver cost 的係數
	// fc * 油耗(L)就是 fuel cost, fd * 工時(H) 就是 driver cost
	fc_ = 1.4;
	fd_ = 8;

	cout << "Problem set ------- OK" << endl;
	//cout << "Please Enter to continue..." << endl;  getchar();

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

bool CProblemSelf::Dp2Object(CIndividual *indv, double obj1_rate) const
{
	const BProblem &prob = *this;
	const double obj1 = obj1_rate, obj2 = 1 - obj1;
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
						//min_value = dp[j][car - 1] + [dis_board[j][i - 1] ;
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

//bool CProblemSelf::PRPDP(CIndividual *indv, double obj1_rate, const double max_fuel, const double max_time) const
//{
//	const BProblem &prob = *this;
//	const double obj1 = obj1_rate, obj2 = 1 - obj1;
//	// const int weight_punishment = 1e8; //目前還不知道要不要用懲罰，懲罰多少?
//
//	const CIndividual::TDecVec &x = indv->vars();
//	const vector<Node> & n = prob.node();
//	const CIndividual::TObjVec &speeds = indv->speed();
//
//	// 計算各段的fuel consumed, 表格是 0-X-0 的油耗
//	// 要有距離、載重、速度
//	// 速度要一段一段乘
//	vector<vector<double>> fuel_board;
//	for (size_t i = 0; i < prob.num_node() - 1; i++)
//	{
//		fuel_board.push_back(vector<double>());
//		for (size_t j = 0; j < prob.num_node() - 1; j++)
//		{
//			fuel_board[i].push_back(0.0);
//		}
//	}
//
//
//	// --- 計算各段的time 
//	
//	// --- initial time_board ---  
//	vector<vector<double>> time_board;
//	for (size_t i = 0; i < prob.num_node() - 1; i++)
//	{
//		time_board.push_back(vector<double>());
//		for (size_t j = 0; j < prob.num_node() - 1; j++)
//		{
//			time_board[i].push_back(0.0);
//		}
//	}
//	// --- calculate time_board --- XXXX
//	size_t next_index = 0, now_index = 0;
//	double time_tmp = 0.0;
//
//	for (size_t i = 0; i<x.size(); i++)
//	{
//		for (size_t j = i; j < x.size(); j++)
//		{
//
//		}
//		if (routes[i] == depot())
//		{
//			time_tmp += start_time_;
//			//cout << "depot = " << i << endl;
//			next_index = i;
//			int wait_time = 0;
//			for (size_t j = now_index; j<next_index; j++)
//			{
//				// ---- calculate waiting time -----
//
//				if (f[1] < node_[routes[j]].ready_time)
//				{
//					wait_time = node_[routes[j]].ready_time - f[1];
//				}
//				else wait_time = 0;
//
//				// ---- calculate total time -----
//				//f[1] += wait_time + node_[routes[j]].service_time + distance_[routes[j]][routes[j + 1]] / (speed[j]*1000/3600);
//				f[1] += wait_time + node_[routes[j]].service_time + distance_[routes[j]][routes[j + 1]] / (speed[j]);
//				//if (total_distance > 646 && total_distance < 648)
//				//{
//				//	cout << "route = " << routes[j] << " " << routes[j + 1] << endl;
//				//	cout << "wait time = " << wait_time << endl;
//				//	cout << "service time = " << node_[routes[j]].service_time << endl;
//				//	cout << "dis = " << distance_[routes[j]][routes[j + 1]] << endl;
//				//	cout << "speed = " << speed[j] << endl;
//				//	cout << "walk time = " << (double)distance_[routes[j]][routes[j + 1]] / (speed[j]) << endl;
//				//	cout << "f[1] += " << wait_time + node_[routes[j]].service_time + distance_[routes[j]][routes[j + 1]] / speed[j] << " = " << f[1] << endl;
//				//	cout << "time = " << f[1] << endl; //getchar(); 
//				//}
//
//			}
//			f[1] -= start_time_;
//			//cout << "final f[1] = " << f[1] << endl;
//			now_index = next_index;
//		}
//	}
//
//
//	// DP 
//
//	// 插入倉庫
//
//	return this->EvaluateOldEncoding(indv);
//}
// -----------------------------------------------------------
double CProblemSelf::Calculate_distance(const CIndividual::TDecVec &routes,int s,int e) const
{
	//計算一條route的距離,s,e不包含倉庫點
	double distance = distance_[depot_section_][routes[s]] + distance_[routes[e]][depot_section_];
	for (int i = s; i < e; i++)
	{
		distance += distance_[routes[i]][routes[i + 1]];
	}
	return distance; // 直接回傳是 公尺(m)
}
// -----------------------------------------------------------
double CProblemSelf::Calculate_time(const CIndividual *indv, int s, int e) const
{
	//計算一條route的時間,s,e不包含倉庫點
	const CIndividual::TDecVec &routes = indv->routes();
	const CIndividual::TObjVec &speed = indv->speed();
	double time = distance_[depot_section_][routes[s]]/speed[s-1] ;
	for (int i = s; i <= e; i++)
	{
		// ---- check time window----- too early 
		double wait_time = 0;
		if (time < node_[routes[i]].ready_time) 
		{
			wait_time = node_[routes[i]].ready_time - time;
		}
		else if (time > node_[routes[i]].due_time)
		{
			//wait_time = 1e7;
			/*cout << "routes i  = " << routes[i] << endl;
			cout << "ar time = " << time << endl;
			cout << "invalid time !!" << endl;  getchar();*/
		}
		else wait_time = 0;

		double s_time = node_[routes[i]].service_time, // service time
			walk_time = distance_[routes[i]][routes[i + 1]] / speed[i];
				
		// ---- total time = waiting time + service time + walk time -----
		//time += wait_time + node_[routes[i]].service_time + distance_[routes[i]][routes[i + 1]] / (speed[i]*1000/3600);
		time += wait_time + s_time + walk_time;
		//cout << "~~time == " << time << endl;
	}
	if (time > node_[depot_section_].due_time)
	{
		//cout << "depot ar time = " << time << endl;
		cout << "invalid time !!" << endl;  getchar();
	}
	time -= start_time_; // 這不算在我服務的時間
	//cout << "time = " << time << endl;
	return time; // 直接回傳是 秒(s)
}
// -----------------------------------------------------------


bool CProblemSelf::EvaluateOldEncoding(CIndividual *indv) const
{
	// ----- initial setting ------
	const CIndividual::TDecVec &routes = indv->routes();
	const CIndividual::TDecVec &x = indv->vars();
	const CIndividual::TObjVec &speed = indv->speed();
	CIndividual::TObjVec &f = indv->objs();
	const double infeasible_value = 1e8;

	// ----- Check if solution is valid. -----
	vector <bool> check_gene;
	check_gene.resize(routes.size(), false);
	bool continous_depot = false;
	double load_check = 0.0;
	int vehicle_num = 0;
	//cout << "\nEva :\n";
	for (size_t i = 0; i<routes.size(); i++)
	{
		//cout << "x[i] = " << x[i] << endl;
		if (routes[i] == depot()) {
			//cout << "1" << endl;
			vehicle_num++;
			load_check = 0;
			if (continous_depot)
			{
				f[0] = infeasible_value;
				f[1] = infeasible_value;
				cout << "routes = " << routes[i - 1] << ' ' << routes[i] << endl;
				cout << "continous Invalid !" << endl;
				cout << "Please Enter to continue ... " << endl; getchar();
				return false;
			}
			continous_depot = true;
		}
		else if (check_gene[routes[i]])
		{
			//cout << "2" << endl;
			f[0] = infeasible_value;
			f[1] = infeasible_value;
			cout << "same customers Invalid !" << endl;
			cout << "Please Enter to continue ... " << endl; getchar();
			return false;
		}
		else
		{
			//cout << "3" << endl;
			load_check += node_[routes[i]].demand;
			//cout << "load = " << load_check << endl;
			if (load_check > maxload_)
			{
				f[0] = infeasible_value;
				f[1] = infeasible_value;
				cout << "load Invalid !" << endl;
				cout << "Please Enter to continue ... " << endl; getchar();
				return false;
			}
			check_gene[routes[i]] = true;
			continous_depot = false;
		}
	}
	// ----- set the vehicle num -----
	indv->set_num_vehicles(vehicle_num - 1);

	//cout << "vnum = " << indv->num_vehicles() << endl; getchar();
	//cout << "EVA 2" << endl; getchar();

	// You can define your own problem here.

	//int limit_vehicles = (int)indv->num_vehicles() - (int)this->num_vehicles(); //現在沒有車輛限制
	//f[0] = max((int)indv->num_vehicles() - (int)this->num_vehicles(), static_cast<size_t>(0))*5e3;
	//f[0] = max(limit_vehicles, static_cast<int>(0))*5e3; //現在沒有車輛限制

	// ----- object 1 (fuel consumed) : total fuel (version 2)-----
	/*************************************************************************
	Objective 1-1
	L當成lamda
	總合kNVLd x 總合z/v
	k = 0.2(引擎磨擦係數), N = 33(引擎轉速)(轉/秒),V = 5(發動機排量)
	L = 1/44x737 = 0.00003083
	d(距離)，總合z/v = 1/速度
	==> kNVL= 0.2 x 33 x 5 x 0.00003083 = 0.00101739
	==> 0.00101739 * d * 1/速度

	Objective 1-2
	總合wrLadx
	r = gama = 1/(1000 x 0.4 x 0.9) = 0.0027778
	w = 本身車重 = 6350 kg
	L = 0.00003083
	a = 0 + 0 + 9.81 x 0.01 x 1 = 0.0981
	x = 要不要走ij
	d(距離)
	==> 0.0027778 x 6350 x 0.00003083 x 0.0981 x d
	==> 0.000053348 x d

	Objective 1-3
	就是把1-2 換成載重
	==> 0.0027778 x f x 0.00003083 x 0.0981 x d
	==> 0.00000008401 * f * d

	Objective 1-4
	總合BrLd總合z/v^2
	B = 0.5 x 0.7 x 1.2041 x 3.912 = 1.64865372
	r = 0.0027778
	L = 0.00003083
	==> 1.64865372 x 0.0027778 x 0.00003083 x d x 1/v^2
	==> 0.00000014119 *d/v^2

	會被變動的就是 d(距離) v(速度) f(載重)
	其中也會紀錄下total distance
	*************************************************************************/

	// ----- object 1 (fuel consumed) : total fuel (version 1)-----
	/*************************************************************************
	Objective 1 原始公式如下:
	F(v,M) = lamda(kNV + wrav + rafv + Brv^3)d/v
	需要代入的只有v和M也就是速度和重量
	M = w(車子原重) + f(貨物重量)
	L當成lamda，L = 1/44x737 = 0.00003083
	kNV :
	k = 0.2(引擎磨擦係數), N = 33(引擎轉速)(轉/秒),V = 5(發動機排量)
	==> kNV = 0.2 x 33 x 5 = 33
	wrav :
	w = 本身車重 = 6350 kg
	r = gama = 1/(1000 x 0.4 x 0.9) = 0.0027778
	a = 0 + 0 + 9.81 x 0.01 x 1 = 0.0981
	==> wra = 6350 x 0.0027778 x 0.0981 = 1.73088843
	==> 等同於 1.73088843 x v
	rafv :
	r = gama = 1/(1000 x 0.4 x 0.9) = 0.0027778
	a = 0 + 0 + 9.81 x 0.01 x 1 = 0.0981
	==> ra = 0.00027250218
	==> 等同於 0.00027250218 x f x v
	Brv^3 :
	B = 0.5 x 0.7 x 1.2041 x 3.912 = 1.64865372
	r = 0.0027778
	==> Br = 0.004579630303416
	==> 等同於 0.004579630303416 x v^3
	最後整合 :
	==> 0.00003083(33 + 1.73088843 x v + 0.00027250218 x f x v + 0.004579630303416 x v^3)d/v

	會被變動的就是 d(距離) v(速度) f(載重)
	其中也會紀錄下total distance
	*************************************************************************/
	//cout << "Obj1 version 2" << endl;
	double next_posx, next_posy,
		now_pos = depot_section_, next_pos; //現在不是從1開始，都是從0開始(depot_section)
	//vector <double> v_dis; //用在object 2 (每段距離)
	double total_distance = 0.0,now_load = 0;

	for (size_t i = 1; i<routes.size(); i++)
	{
		//cout << "i = " << i << endl;
		next_pos = routes[i];
		if (now_pos == depot_section_)
		{
			//cout << "in" << endl;
			int tmp = i;
			//now_load = 0;
			// ----- 計算這台車出發時的載重 -----
			while (routes[tmp] != depot_section_)
			{
				//cout << "x[" << tmp << "] = " << x[tmp] << endl;
				//cout << "d = " << node_[x[tmp]].demand << endl;
				now_load += node_[routes[tmp]].demand;
				tmp++;
			}
			//cout << "load = " << now_load << endl;
		}
		//double dis = distance_[now_pos][next_pos]/1000;//因為他是給公尺，所以應該要/1000吧
		double dis = distance_[now_pos][next_pos];
		f[0] += 0.00003083 * (33 + 1.73088843 * speed[now_pos] + 0.00027250218 * now_load * speed[now_pos] + 0.004579630303416 * speed[now_pos] * speed[now_pos] * speed[now_pos]) * (dis) / speed[now_pos];
		//f[0] += 0.00003083 * (33 + 1.73088843 * (speed[now_pos] * 3600 / 1000) + 0.00027250218 * now_load * (speed[now_pos] * 3600 / 1000) + 0.004579630303416 * (speed[now_pos] * 3600 / 1000)* (speed[now_pos] * 3600 / 1000)* (speed[now_pos] * 3600 / 1000)) * (dis) / (speed[now_pos] * 3600 / 1000);
		total_distance += dis/1000;//因為他是給公尺，所以應該要/1000吧
		//cout << "f0 - " << i << " = " << f[0] << endl;
		now_load -= node_[next_pos].demand;
		//cout << "nowload -= " << node_[next_pos].demand << " = " << now_load << endl;
		now_pos = next_pos;
	}
	indv->set_total_dis(total_distance);
	//cout << "total dis = " << total_distance << endl;

	/*double test_dis = 0.0;
	int index_one = 1, index_two = 1;
	for (int i = 1; i < routes.size(); i++)
	{
		if (routes[i] == depot_section_)
		{
			index_two = i-1;
			test_dis += Calculate_distance(routes, index_one, index_two);
			index_one = i + 1;
		}
	}
	cout << "test dis = " << test_dis << endl; getchar();*/
	// 找跟UK10_1相近的解--------------------------------start
	/*if (total_distance > 408 && total_distance < 410)
	{
		cout << "routes : ";
		for (int i = 0; i < routes.size(); i++)
		{
			cout << routes[i] << " ";
		}
		cout << endl;
		cout << "total dis = " << total_distance << endl;
		cout << "fuel = " << f[0] << endl;
		getchar();
	}*/
	//cout << "f[0] = " << f[0] << endl;
	//cout << "total dis = " << total_distance/1000 << "(KM)" << endl; //getchar();
	//cout << "Fuel = " << f[0] << "(L)" << endl; //getchar();
	// 找跟UK10_1相近的解---------------------------------end
	// ----- object 2 (driving time): total using time ------
	/*************************************************************************
		minimize total sj
		sj = cj + tj + dj0/vr
		簡單來說就是所有時間都要算進去 = 回倉庫時間 - 倉庫出發時間 
	*************************************************************************/
	//cout << "Obj 2" << endl; getchar();

	cout << "routes : ";
	for (int i = 0; i < routes.size(); i++)
	{
		cout << routes[i] << " ";
	}
	cout << endl;

	size_t next_index = 0, now_index = 0;
	for (size_t i = 1; i<routes.size(); i++)
	{
		if (routes[i] == depot())
		{
			cout << "two" << endl;
			double route_time = start_time_;
			//f[1] += start_time_;
			//cout << "depot = " << i << endl;
			next_index = i;
			double wait_time = 0;
			for (size_t j = now_index; j<next_index; j++)
			{
				// ---- calculate waiting time -----
				
				if (route_time < node_[routes[j]].ready_time)
				{
					wait_time = node_[routes[j]].ready_time - route_time;
				}
				else wait_time = 0;
				
				// ---- calculate total time -----
				//f[1] += wait_time + node_[routes[j]].service_time + distance_[routes[j]][routes[j + 1]] / (speed[j]*1000/3600);
				route_time += wait_time + node_[routes[j]].service_time + distance_[routes[j]][routes[j + 1]] / (speed[j]);
				//f[1] += wait_time + node_[routes[j]].service_time + distance_[routes[j]][routes[j + 1]] / (speed[j]);

				//if (total_distance > 646 && total_distance < 648)
				//{
				//	cout << "route = " << routes[j] << " " << routes[j + 1] << endl;
				//	cout << "wait time = " << wait_time << endl;
				//	cout << "service time = " << node_[routes[j]].service_time << endl;
				//	cout << "dis = " << distance_[routes[j]][routes[j + 1]] << endl;
				//	cout << "speed = " << speed[j] << endl;
				//	cout << "walk time = " << (double)distance_[routes[j]][routes[j + 1]] / (speed[j]) << endl;
				//	cout << "f[1] += " << wait_time + node_[routes[j]].service_time + distance_[routes[j]][routes[j + 1]] / speed[j] << " = " << f[1] << endl;
				//	cout << "time = " << f[1] << endl; //getchar(); 
				//}
				
			}
			route_time -= start_time_;
			f[1] += route_time;
			//cout << "route time = " << route_time << endl;
			//cout << "final f[1] = " << f[1] << endl;
			now_index = next_index;
		}
	}
	
	/*if (total_distance > 646 && total_distance < 648) {
		cout << "total dis = " << total_distance << endl;
		getchar();
	}*/
	double test_time = 0.0;
	int index_one = 1, index_two = 1;
	for (int i = 1; i < routes.size(); i++)
	{
		if (routes[i] == depot_section_)
		{
			//cout << "one" << endl;
			index_two = i - 1;
			test_time += Calculate_time(indv, index_one, index_two);
			index_one = i + 1;
			//cout << "test time = " << test_time << endl; getchar();
		}
	}
	//cout << "= " << f[1] / 3600 << "(h)" << endl; //getchar();

	return true;
}
