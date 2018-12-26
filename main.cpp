#include "problem_base.h"
#include "alg_nsgaiii.h"
#include "alg_population.h"
#include "exp_experiment.h"

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <set>

#include "ctimer.h"

#include "gnuplot_interface.h"
#include "log.h"
#include "aux_misc.h"
#include "exp_indicator.h"

using namespace std;

class Point_set
{
public:
	double x, y;
	bool operator < (const Point_set & tmp) const
	{
		if (x == tmp.x)
		{
			return y < tmp.y;
		}
		return (x < tmp.x);
	}
};

bool OutputChromesome(const CIndividual &chrome, const string file_name, const BProblem &prob);

int main()
{
	ifstream exp_list("explist.ini");
	if (!exp_list) { cout << "We need the explist.ini file." << endl; getchar(); return 1; }

	string exp_name;
	while (exp_list >> exp_name)
	{
		cout << "exp_name = " << exp_name << endl;
		ifstream exp_ini("Experiments\\" + exp_name); //exp_name = xxx.ini
		if (!exp_ini) { cout << exp_name << " file does not exist." << endl; getchar();  continue; }
		// ----- Setup the expriment ------
		cout << "Setup the expirement" << endl;
		CNSGAIII nsgaiii;
		BProblem *problem = 0;
		//set<Point_set> ps; //用來看目前有幾個不同個體

		SetupExperiment(nsgaiii, &problem, exp_ini);
		//cout << "---- Setup finish ----" << endl; getchar();
		Gnuplot gplot;
		ofstream IGD_results(nsgaiii.name() + "-" + problem->name() + "-IGD.txt"); // output file for IGD values per run

																				   // ----- Run the algorithm to solve the designated function -----
		const size_t NumRuns = 10; // 20 is the setting in NSGA-III paper
		for (size_t r = 0; r<NumRuns; r += 1)
			/*bool set_check = false;
			const size_t set_num = 1000000;
			size_t r = 0;
			while(!set_check)*/
		{
			//r++; //wu cnt 
			srand(r); cout << "Solving " << problem->name() << " ... Run: " << r << endl;
			// --- Write number of runs to problem.txt
			fstream file;
			string file_name = problem->name() + ".txt";
			file.open(file_name, ios::out | ios::app);
			if (!file) cout << "Can't open " << file_name << endl;
			file << r + 1 << " Runs" << endl;
			file.close();

			// --- Solve
			CPopulation solutions; //pop.size() = 0
			nsgaiii.Solve(&solutions, *problem);
			// --- Output the result
			string logfname = "Results\\" + nsgaiii.name() + "-" + problem->name() + "-Run" + IntToStr(r) + ".txt"; // e.g. NSGAIII-DTLZ1(3)-Run0.txt
			SaveScatterData(logfname, solutions);

			// --- Calculate the performance metric (IGD)
			TFront PF, approximation;
			IGD_results << IGD(LoadFront(PF, "PF\\" + problem->name() + "-PF.txt"), LoadFront(approximation, logfname)) << endl;

			// --- Visualization (Show the last 3 dimensions. You need gnuplot.)
			//ShowPopulation(gplot, solutions, "gnuplot-show"); system("pause");

			// --- Output the point file and Output min distance and min emission chromesome to (problem)-solution.txt
			fstream point_file;
			string point_file_name = "Point\\" + problem->name() + "-point.txt";
			point_file.open(point_file_name, ios::out | ios::app);
			if (!point_file)
			{
				cout << "Can't write point file!" << endl;
				getchar();
			}
			double min_obj1 = solutions[0].objs()[0], min_obj2 = solutions[0].objs()[1];
			int min_obj1_index = 0, min_obj2_index = 0;
			for (size_t i = 0; i < solutions.size(); i++)
			{
				point_file << solutions[i].objs()[0] << ' ' << solutions[i].objs()[1] << endl;

				if (solutions[i].objs()[0] < min_obj1)
				{
					min_obj1 = solutions[i].objs()[0];
					min_obj1_index = i;
				}
				if (solutions[i].objs()[1] < min_obj2)
				{
					min_obj2 = solutions[i].objs()[1];
					min_obj2_index = i;
				}
			}
			point_file.close();

			cout << "vehicle = " << solutions[min_obj1_index].num_vehicles() << endl;
			printf("min fuel (%d) = %.3lf(L)\n", min_obj1_index, min_obj1);
			printf("time = %.3lf(h)\n", solutions[min_obj1_index].objs()[1] / 3600);
			printf("this distance = %.3lf(km)\n\n", solutions[min_obj1_index].total_dis());
			printf("min time (%d) = %.3lf(h)\n", min_obj2_index, min_obj2 / 3600);
			printf("fuel (%d) = %.3lf(L)\n", min_obj2_index, solutions[min_obj2_index].objs()[0]);
			printf("this distance = %.3lf(km)\n", solutions[min_obj2_index].total_dis());
			//getchar();
			// --- output the 2 best chromesomes to solution.txt
			OutputChromesome(solutions[min_obj1_index], file_name,*problem);
			OutputChromesome(solutions[min_obj2_index], file_name,*problem);

			// --- input the data and output the point in std::set
			//for (size_t s = 0; s < solutions.size(); s++)
			//{
			//	Point_set tmp;
			//	tmp.x = solutions[s].objs()[0];
			//	tmp.y = solutions[s].objs()[1];
			//	if (tmp.x < 5000)
			//	{
			//		ps.insert(tmp);
			//	}
			//}
			//if (ps.size() >= set_num)
			//{
			//	fstream set_file;
			//	string set_name = "random_set\\" + problem->name() + "-set.txt";
			//	set_file.open(set_name, ios::out | ios::app);
			//	set <Point_set>::iterator iter;
			//	for ( iter = ps.begin(); iter != ps.end(); iter++)
			//	{
			//		set_file << (*iter).x << ' ' << (*iter).y << endl;
			//	}
			//	ps.clear();
			//	cout << "total Num Runs = " << r << endl;
			//	set_file.close();
			//	set_check = true;
			//	//MyTimers.print();
			//}
			//if (ps.size() % 10000 == 0)
			//{
			//	cout << "ps size = " << ps.size() << endl;
			//}
			//getchar();
		}
		delete problem;

	}// while - there are more experiments to carry out

	return 0;
}

bool OutputChromesome(const CIndividual &chrome, const string file_name,const BProblem &prob)
{
	fstream outfile;

	outfile.open(file_name, ios::out | ios::app);
	if (!outfile) {
		cout << "Can't open " << file_name << endl;
		getchar();
		return false;
	}
	outfile << "x chrome : ";
	for (auto e : chrome.routes())
	{
		outfile << e << " ";
	}
	outfile << endl;
	outfile << "speed : ";
	for (auto e : chrome.speed())
	{
		outfile << e << " ";
	}
	outfile << endl;
	double fuel_consumed = chrome.objs()[0],
		time = chrome.objs()[1] / 3600,
		fuel_cost = fuel_consumed*prob.fc(),
		driver_cost = time * prob.fd(),
		total_cost = fuel_cost + driver_cost;
	outfile << "fuel consumed = " << fuel_consumed << "(L)" << endl;
	outfile << "fuel cost = " << fuel_cost << "($)" <<  endl;
	outfile << "time = " << time << "(H)" << endl;
	outfile << "driver cost = " << driver_cost << "($)" << endl;
	outfile << "distance = " << chrome.total_dis() << "(KM)" << endl;
	outfile << "total cost = " << total_cost << "($)" << endl;


	outfile.close();
	return true;
}