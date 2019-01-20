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
		for (int j = 0; j < num_node_; j++)
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
	//want_speed_ = 25;// 90 km/h = 25 m/s
	//want_speed_ = 20.9294; // 這是v* 對總cost最好的速度
	want_speed_ = 15.3303; // v* 最低油耗
	cout << "Problem set ------- OK" << endl;
	//cout << "Please Enter to continue..." << endl;  getchar();

}
// -----------------------------------------------------------


bool CProblemSelf::PRPDP(CIndividual *indv, double obj1_rate) const
{
	//cout << "start DP " << endl;
	const BProblem &prob = *this;
	const double obj1 = obj1_rate/100, obj2 = 1 - obj1;
	//const int weight_punishment = 1e8; //目前還不知道要不要用懲罰，懲罰多少?
	const double inf_value = 1e12;
	const double inf_load_fuel = 1e8;
	const double inf_load_time = 1e8;

	const CIndividual::TDecVec &x = indv->vars();
	const vector<Node> & n = prob.node();
	const CIndividual::TObjVec &speeds = indv->speed();
	
	//indv->ShowRoute();
	//for (int i = 0; i < indv->speed().size(); i++) // 檢查這裡有沒有動到速度
	//{
	//	cout << "speed " << i << " = " << speeds[i] << endl;
	//	/*if (speeds[i] != 20.9294 && speeds[i] != 15.3303)
	//	{
	//		cout << "size = " << speeds.size() << "i = " << i << endl;
	//		cout << "SOA head this speed is " << speeds[i] << endl; getchar();
	//	}*/
	//}
	indv->routes() = x; // 把x 先塞進routes
	/*cout << "in PRP" << endl;
	cout << *indv << endl;
	indv->ShowRoute();*/
	//cout << "max load = " << prob.maxload() << endl;
	// 計算各段的fuel consumed, 表格是 0-X-0 的油耗
	// 要有距離、載重、速度
	// 速度要一段一段乘
	vector<vector<double>> fuel_board;
	for (size_t i = 0; i < prob.num_node() - 1; i++)
	{
		double temp_load = n[x[i]].demand;
		fuel_board.push_back(vector<double>());
		for (size_t j = 0; j < prob.num_node() - 1; j++)
		{
			if (i <= j)
			{
				if (i != j) temp_load += n[x[j]].demand;
				if (temp_load > prob.maxload())
				{
					fuel_board[i].push_back(Calculate_fuel(indv, i, j, 1) + inf_load_fuel);
				}
				else
				{
					//cout << "fuel = " << Calculate_fuel(indv, i, j) << endl;
					fuel_board[i].push_back(Calculate_fuel(indv, i, j, 1));
				}
			}
			else
			{
				fuel_board[i].push_back(inf_value);
			}
		}
	}
	/*cout << "fuel board :" << endl;
	for (int i = 0; i<fuel_board.size(); i++)
	{
		for (int j = 0; j<fuel_board.size(); j++)
		{
			cout << fuel_board[i][j] << ' ';
		}
		cout << endl;
	}*/

	// --- 計算各段的time 
	// --- initial time_board ---  
	vector<vector<double>> time_board;
	double normalize_time = 500;
	for (size_t i = 0; i < prob.num_node() - 1; i++)
	{
		time_board.push_back(vector<double>());
		double temp_load = n[x[i]].demand;
		double temp_time = 0.0;
		for (size_t j = 0; j < prob.num_node() - 1; j++)
		{
			if (j >= i)
			{
				if (i != j) temp_load += n[x[j]].demand;
				if (temp_load > prob.maxload())
				{
					//cout << "over load time," << i << ' ' << j << endl;
					time_board[i].push_back(Calculate_time(indv, i, j, 1)/normalize_time + inf_load_time);
				}
				else
				{
					temp_time = Calculate_time(indv, i, j, 1);
					if (temp_time < 0)
					{
						time_board[i].push_back(time_punishment_);
					}
					else
					{
						time_board[i].push_back(temp_time / normalize_time);
					}
					
				}
			}
			else
			{
				time_board[i].push_back(inf_value);
			}
		}
	}
	/*cout << "time board :" << endl;
	for (int i = 0; i<time_board.size(); i++)
	{
	for (int j = 0; j<time_board.size(); j++)
	{
	cout << time_board[i][j] << ' ';
	}
	cout << endl;
	}*/
	//getchar();
	// DP ------------------------------------ up
	// initial 
	vector<double> dp; //1維DP 
	vector<vector<int>> cut_point;
					   //initial dp  = 0.0---------------------------------------------
	dp.push_back(obj1 * fuel_board[0][0] + obj2 * time_board[0][0]);
	cut_point.push_back(vector<int>());
	/*cout << "obj1 = " << obj1 << endl;
	cout << "obj2 = " << obj2 << endl;
	cout << "f 0 0 = " << fuel_board[0][0] << endl;
	cout << "t 0 0 = " << time_board[0][0] << endl;
	cout << "dp 0 = " << dp[0] << endl;*/
	for (size_t i = 1; i < prob.num_node()-1; i++)
	{
		double dp_value = obj1 * fuel_board[0][i] + obj2 * time_board[0][i];
		if (time_board[0][i] < 0)
		{
			dp_value = time_punishment_;
		}
		cut_point.push_back(vector<int>());
		vector<int> tmp_cut;
		bool cut_check = false;
		//cout << "i = " << i << endl;
		//cout << "cut size = " << cut_point.size() << endl;
		for (size_t j = 0; j < i; j++)
		{
			//cout << "j = " << j << endl;
			double temp_value = dp[i - j - 1] + obj1 * fuel_board[i - j][i] + obj2 * time_board[i - j][i];
			if (time_board[i-j][i] > 0 &&   temp_value < dp_value)
			{
				if (time_board[i - j][i] >= 1e8)
				{
					cout << "time_board[i-j][i] = " << time_board[i - j][i] << endl;
					cout << "obj2 = " << obj2 << endl;
				}
				
				//cout << "dp = " << dp_value << ' ' << "temp = " << temp_value << endl;
				dp_value = temp_value;
				if (cut_point[i - j - 1].size())
				{
					tmp_cut = cut_point[i - j - 1];
					//cout << "tmp cut = ";
					/*for (int k = 0; k < tmp_cut.size(); k++)
					{
						cout << tmp_cut[k] << ' ';
					}
					cout << endl;*/
				}
				else
				{
					tmp_cut.resize(0);
				}
				tmp_cut.push_back(i - j - 1);
				//cout << "new cut = " << i - j - 1 << endl;
				cut_check = true;
			}
		}
		if (tmp_cut.size())
		{
			cut_point[i] = tmp_cut;
		}
		
		dp.push_back(dp_value);
	}
	//cout << "OK dp board" << endl;
	/*cout << "cut point : " << endl;
	for (int j = 0; j < cut_point.size(); j++)
	{
		cout << "cutpoint[" << j << "]: ";
		for (int i = 0; i < cut_point[j].size(); i++)
		{
			cout << cut_point[j][i] << ' ';
		}
		cout << endl;
	}
	getchar();*/

	/*cout << "cutpoint[]: ";
	for (int i = 0; i < cut_point[cut_point.size()-1].size(); i++)
	{
		cout << cut_point[cut_point.size() - 1][i] << ' ';
	}
	cout << endl; getchar();*/
	//cout << "dp board : " << endl;
	//for (int i = 0; i<dp.size(); i++)
	//{
	//	/*if (dp[i] > 1e5)
	//	{
	//		cout << "dp[" << i << "] = " << dp[i] << endl; getchar();
	//	}*/
	//	cout << "dp[" << i << "] = " << dp[i] << endl;
	//}
	//cout << endl; //getchar();
	/*cout << "cut size = " << cut_point[cut_point.size() - 1].size() << endl;
	if (cut_point[cut_point.size() - 1].size() == 1)
	{
		cout << "I don't cut.." << endl;
		cout << "I cut :" << cut_point[cut_point.size() - 1][0] << endl;
		cout << "dp board : " << endl;
		for (int i = 0; i<dp.size(); i++)
		{
		cout << "dp[" << i << "] = " << dp[i] << endl;
		}
		cout << endl; getchar();
	}*/
	// DP ------------------------------------ down
	// 插入倉庫
	
	CIndividual::TDecVec &routes = indv->routes();
	routes.insert(routes.begin(),depot());
	for (int i = 0; i < cut_point[cut_point.size() - 1].size(); i++)
	{
		routes.insert(routes.begin() + cut_point[cut_point.size() - 1][i] + 1 + i + 1, prob.depot());
	}
	routes.push_back(depot());

	while (indv->speed().size() < routes.size() - 1)//把speed 補好
	{
		cout << indv->speed().size() << '<' << routes.size() - 1 << endl;
		cout << "what??" << endl; getchar();
		indv->speed().push_back(want_speed());
	}

	//for (int i = 0; i < indv->speed().size(); i++) // 速度都設定成 avg speed
	//{
	//	if (indv->speed()[i] != want_speed())
	//	{
	//		cout << "not equel to want speed " << endl;
	//		cout << "speed = " << indv->speed()[i] << endl;  getchar();
	//		indv->speed()[i] = want_speed();
	//	}
	//}

	// 檢查這裡有沒有動到速度
	/*for (int i = 0; i < indv->speed().size(); i++) 
	{
		if (speeds[i] != 20.9294 && speeds[i] != 15.3303)
		{
			cout << "i = " << i << endl;
			cout << "PRP down speed is " << speeds[i] << endl; getchar();
		}
	}*/
	//indv->ShowRoute();
	//getchar();
	//cout << "dp ok " << endl; //getchar();

	return this->EvaluateOldEncoding(indv);
}
// -----------------------------------------------------------
double CProblemSelf::Calculate_distance(const CIndividual::TDecVec &routes, int s, int e) const
{
	//計算一條route的距離,s,e不包含倉庫點, 目前只有可能拿來算完整情況,所以就不用x了
	double distance = distance_[depot_section_][routes[s]] + distance_[routes[e]][depot_section_];
	//cout << "d->s = " << distance_[depot_section_][routes[s]] << endl;
	//cout << "e->d = " << distance_[routes[e]][depot_section_] << endl;
	for (int i = s; i < e; i++)
	{
		//cout << routes[i] << "-" << routes[i+1] << "=" <<  distance_[routes[i]][routes[i + 1]] << endl;
		distance += distance_[routes[i]][routes[i + 1]];
	}
	//cout << "dis = " << distance << endl;
	return distance; // 直接回傳是 公尺(m)
}
// -----------------------------------------------------------
double CProblemSelf::Calculate_time(CIndividual *indv, int s, int e, int depot_num) const
{
	// ----- object 2 (driving time): total using time ------
	/*************************************************************************
	minimize total sj
	sj = cj + tj + dj0/vr
	簡單來說就是所有時間都要算進去 = 回倉庫時間 - 倉庫出發時間
	*************************************************************************/
	//計算一條route的時間,s,e不包含倉庫點
	const CIndividual::TDecVec &x = indv->vars();
	const CIndividual::TObjVec &speed = indv->speed();
	double speed_now = speed[s + depot_num - 1];
	double time = distance_[depot_section_][x[s]] / speed_now;
	bool feasible_route = true;
	/*cout << depot_section_ << " > " << x[s] << endl;
	cout << "start dis = " << distance_[depot_section_][x[s]] << endl;
	cout << s + depot_num - 1 << " start speed = " << speed_now << endl;
	cout << "start -> " << time << endl;
	cout << s + depot_num -1<< "speed = " << speed_now << endl;*/
	for (int i = s; i <= e; i++)
	{
		// ---- check time window----- too early 
		//cout << "i = " << i << endl;
		speed_now = speed[i + depot_num];
		//cout << i + depot_num << "speed = " << speed_now << endl;
		
		double wait_time = 0;
		if (time < node_[x[i]].ready_time)
		{
			wait_time = node_[x[i]].ready_time - time;
		}
		else if (time > node_[x[i]].due_time)
		{
			//cout << i + depot_num << " normal bad time " << endl;
			wait_time = time_punishment_;
			feasible_route = false;
			//indv->set_feasible(false);
			//cout << "invalid time !!" << endl;  //getchar();
		}
		else wait_time = 0;

		
		double s_time = node_[x[i]].service_time, // service time
			walk_time; // 行駛時間

		if (i < e)
		{
			//cout << x[i] << " > " << x[i + 1] << endl;
			walk_time = distance_[x[i]][x[i + 1]] / speed_now;
		}
		else // i == e
		{
			//cout << x[i] << " > " << depot_section_ << endl;
			walk_time = distance_[x[i]][depot_section_] / speed_now;
		}
		/*if (walk_time > 10000)
		{
			cout << "big walk time :" << walk_time << endl;
			cout << "dis 1 = " << distance_[x[i]][x[i + 1]] << endl;
			cout << "dis = " << distance_[x[i]][depot_section_] << endl;
			cout << "speed = " << speed_now << endl;
			getchar();
		}*/
		/*if (wait_time > 10000)
		{
			cout << "big wait time :" << wait_time << endl;
			getchar();
		}*/
		/*cout << "w time = " << walk_time << endl;
		cout << "s time = " << s_time << endl;*/
		// ---- total time = waiting time + service time + walk time -----
		//time += wait_time + node_[x[i]].service_time + distance_[x[i]][x[i + 1]] / (speed[i]*1000/3600);
		time += wait_time + s_time + walk_time;
		//cout << "this time = " << time << endl;// getchar();

	}
	//cout << "speed = " << speed_now << endl;
	if (time > node_[depot_section_].due_time)
	{
		//cout << "to depot bad time" << endl;
		time += time_punishment_;
		feasible_route = false;
		//indv->set_feasible(false);
		//cout << "invalid time !!" << endl; // getchar();
	}
	time -= start_time_; // 這不算在我服務的時間

	//cout << "time = " << time << endl; //getchar();
	if (time > 100000 && feasible_route)
	{
		cout << "time = " << time << endl;
	}

	if (! feasible_route)
	{
		//cout << "infea time = " << time << endl;
		return -1;
	}

	return time; // 直接回傳是 秒(s)
}
// -----------------------------------------------------------
double CProblemSelf::Calculate_fuel(const CIndividual *indv, int s, int e, int depot_num) const
{
	//cout << "start fuel cal" << endl;
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
	//計算一條route的油耗,s,e不包含倉庫點
	const CIndividual::TDecVec &x = indv->vars();
	const CIndividual::TDecVec &routes = indv->routes();
	const CIndividual::TObjVec &speed = indv->speed();
	double fuel = 0.0, load = 0.0;
	// 初始化發車時的載重
	for (int i = s; i <= e; i++)
	{
		load += node_[x[i]].demand;
	}
	if (load > maxload_)
	{
		//cout << "WTFFFFF load !" << endl; //getchar();
		fuel = fuel_punishment_;
		//cout << "fuel " << fuel << endl;
		//indv->set_feasible(false);
	}
	// 倉庫出發
	double speed_now = speed[s + depot_num - 1];
	fuel = 0.00003083 * (33 + 1.73088843 * speed_now + 0.00027250218 * load * speed_now + 0.004579630303416 * speed_now * speed_now * speed_now) * (distance_[depot_section_][x[s]]) / speed_now;
	for (int i = s; i < e; i++)
	{
		speed_now = speed[i + depot_num];
		load -= node_[x[i]].demand;
		// speed 單位= m/s
		fuel += 0.00003083 * (33 + 1.73088843 * speed_now + 0.00027250218 * load * speed_now + 0.004579630303416 * speed_now * speed_now * speed_now) * (distance_[x[i]][x[i + 1]]) / speed_now;
		/*cout << "speed = " << speed_now << endl;
		cout << "fuel = " << fuel << endl;*/
		// speed 單位 = km/h
		//f[0] += 0.00003083 * (33 + 1.73088843 * (speed[now_pos] * 3600 / 1000) + 0.00027250218 * now_load * (speed[now_pos] * 3600 / 1000) + 0.004579630303416 * (speed[now_pos] * 3600 / 1000)* (speed[now_pos] * 3600 / 1000)* (speed[now_pos] * 3600 / 1000)) * (dis) / (speed[now_pos] * 3600 / 1000);
	}
	speed_now = speed[e + depot_num];
	load -= node_[x[e]].demand;
	fuel += 0.00003083 * (33 + 1.73088843 * speed_now + 0.00027250218 * load * speed_now + 0.004579630303416 * speed_now * speed_now * speed_now) * (distance_[x[e]][depot_section_]) / speed_now;
	//cout << "fuel = " << fuel << endl; getchar();
	return fuel; // 直接回傳是 公升(L) ?
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
	indv->set_feasible(true);
	//indv->ShowRoute();

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
				indv->set_feasible(false);
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
			indv->set_feasible(false);
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
				//indv->ShowRoute();
				cout << "load Invalid !" << endl;
				//cout << "Please Enter to continue ... " << endl; getchar();
				indv->set_feasible(false);
				return false;
			}
			check_gene[routes[i]] = true;
			continous_depot = false;
		}
	}
	// ----- set the vehicle num -----
	indv->set_num_vehicles(vehicle_num - 1);

	//cout << "vnum = " << indv->num_vehicles() << endl; getchar();

	// You can define your own problem here.

	//int limit_vehicles = (int)indv->num_vehicles() - (int)this->num_vehicles(); //現在沒有車輛限制
	//f[0] = max((int)indv->num_vehicles() - (int)this->num_vehicles(), static_cast<size_t>(0))*5e3;
	//f[0] = max(limit_vehicles, static_cast<int>(0))*5e3; //現在沒有車輛限制
	// print x and routes 

	//cout << *indv << endl; // print x 
	//indv->ShowRoute(); // print route
	/*cout << "speed : ";
	for (int i = 0; i < speed.size(); i++)
	{
		cout << speed[i] << ' ';
	}
	cout << endl;*/
	//cout << "if feasible : " << indv->feasible() << endl;
	double total_fuel = 0.0, total_distance = 0.0, total_time = 0.0;
	int head = 1, tail = 1, depot_cnt = 1;
	for (int i = 1; i < routes.size(); i++)
	{
		if (routes[i] == depot_section_)// 一次算一條route
		{
			//cout << "route[" << i << "]=" << routes[i] << endl;
			tail = i - 1;
			//cout << "head = " << head - depot_cnt << endl;
			//cout << "tail = " << tail - depot_cnt << endl;
			//cout << "fuel = " << head << " " << tail << " = " << head - depot_cnt << " " << tail - depot_cnt << endl;
			total_fuel += Calculate_fuel(indv, head - depot_cnt, tail - depot_cnt, depot_cnt);
			//cout << "if feasible after fuel: " << indv->feasible() << endl;
			//cout << "distance" << head << " " << tail << " = " << head - depot_cnt << " " << tail - depot_cnt << endl;
			total_distance += Calculate_distance(routes, head, tail) / 1000; //m => km
																			 //cout << "time" << head << " " << tail << " = " << head - depot_cnt << " " << tail - depot_cnt << endl;
			//cout << "if feasible after dis: " << indv->feasible() << endl;
			double temp_time = Calculate_time(indv, head - depot_cnt, tail - depot_cnt, depot_cnt);
			if (temp_time < 0) {
				indv->set_feasible(false);
				//cout << "head = " << head << ' ' << "tail = " << tail << endl;
				//cout << "time problem" << endl;
				total_time += time_punishment_;
			}
			else
			{
				total_time += Calculate_time(indv, head - depot_cnt, tail - depot_cnt, depot_cnt);
			}
			
			//cout << "if feasible after time: " << indv->feasible() << endl;
			//cout << "OK" << endl;
			head = i + 1;
			depot_cnt++;
			//getchar();
		}
	}
	indv->set_total_dis(total_distance);
	//if (total_distance < 490)
	//{
	//	cout << "total dis = " << total_distance << endl; //getchar();
	//}
	
	f[0] = total_fuel;
	f[1] = total_time;
	
	//cout << "fuel = " << total_fuel << endl;
	//cout << "total dis = " << total_distance << endl;
	//cout << "total time = " << total_time << endl; getchar();


	// 找跟UK10_1相近的解--------------------------------start
	/*if (total_distance > 408 && total_distance < 410)
	{
	indv->ShowRoute();
	cout << "total dis = " << total_distance << endl;
	cout << "fuel = " << f[0] << endl;
	cout << "time = " << f[1]/3600 << endl;
	cout << "total cost = " << f[0] * 1.4 + f[1]/3600 * 8 << endl;
	cout << "feasible = " << indv->feasible() << endl;
	getchar();
	}*/
	//cout << "f[0] = " << f[0] << endl;
	//cout << "Fuel = " << f[0] << "(L)" << endl; //getchar();
	// 找跟UK10_1相近的解---------------------------------end

	/*cout << "fuel = " << f[0] << endl;
	cout << "time = " << f[1] / 3600 << endl;
	cout << "feasible = " << indv->feasible() << endl; getchar();*/
	//cout << "= " << f[1] / 3600 << "(h)" << endl; //getchar();
	//cout << "almost eva" << endl;
	if (indv->feasible())
	{
		return true;
	}
	else
	{
		//indv->ShowRoute();
		//cout << "final inf" << endl;
		
		//getchar();
		//f[0] = infeasible_value;
		//f[1] = infeasible_value;
		f[0] *= 1.5;
		f[1] *= 1.5;
		/*cout << "f 0 = " << f[0] << endl;
		cout << "f 1 = " << f[1] << endl;
		getchar();*/
		return false;
	}

}
