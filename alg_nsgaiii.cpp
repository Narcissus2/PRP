#include "alg_nsgaiii.h"
#include "problem_base.h"
#include "alg_individual.h"
#include "alg_reference_point.h"
#include "alg_population.h"

#include "alg_initialization.h"
#include "alg_crossover.h"
#include "alg_mutation.h"
#include "alg_environmental_selection.h"

#include "gnuplot_interface.h"
#include "log.h"
#include "windows.h" // for Sleep()
#include "ctimer.h"

#include "alg_refinement.h"

#include <vector>
#include <fstream>

#include <iostream>
#include <stdlib.h>
#include <algorithm>
using namespace std;


CNSGAIII::CNSGAIII() :
	name_("NSGAIII"),
	gen_num_(1),
	pc_(1.0), // default setting in NSGA-III (IEEE tEC 2014)
	eta_c_(30), // default setting
	eta_m_(20) // default setting
{
}
// ----------------------------------------------------------------------
void CNSGAIII::Setup(ifstream &ifile)
{
	if (!ifile) return;

	string dummy;
	ifile >> dummy >> dummy >> name_; //algorithm.name(dummy) =(dummy) NSGAIII (name_)

	size_t p1 = 0, p2 = 0;
	//ifile >> dummy >> dummy >> p1 >> p2; 
	ifile >> dummy >> dummy >> p1;
	obj_division_p_.push_back(p1);

	if (!ifile) ifile.clear();
	//else obj_division_p_.push_back(p2);

	ifile >> dummy >> dummy >> gen_num_;
	ifile >> dummy >> dummy >> pc_;
	ifile >> dummy >> dummy >> eta_c_;
	ifile >> dummy >> dummy >> eta_m_;
}
// ----------------------------------------------------------------------
void CNSGAIII::Solve(CPopulation *solutions, const BProblem &problem)
{
	//MyTimers.GetTimer("AllSolve")->start();
	//MyTimers.GetTimer("SetTargetProblem")->start();
	CIndividual::SetTargetProblem(problem);
	//MyTimers.GetTimer("SetTargetProblem")->end();

	//MyTimers.GetTimer("Reference Point")->start();
	vector<CReferencePoint> rps;
	GenerateReferencePoints(&rps, problem.num_objectives(), obj_division_p_);
	size_t PopSize = rps.size();
	while (PopSize % 4) PopSize += 1;
	//MyTimers.GetTimer("Reference Point")->end();

	//MyTimers.GetTimer("CPopulation")->start();
	CPopulation pop[2] = { CPopulation(PopSize) };
	string file_name = problem.name() + ".txt";
	const string gen_record = problem.name() + "_gen_record.txt";
	//MyTimers.GetTimer("CPopulation")->end();

	//CSimulatedBinaryCrossover SBX(pc_, eta_c_);
	//CPartiallyMappedCrossover PMX(pc_);
	COrderedCrossover OX(pc_);
	CLinearOrderedCrossover LOX(pc_);
	//CPolynomialMutation PolyMut(1.0/problem.num_variables(), eta_m_);
	//CRandomTwoSwapMutationDepot R2S(0.05);
	CRandomTwoSwapMutation swap_mutation(0.05);
	CRandomTwoPointMutation two_point_mutation(1);
	COnePointOpt OPO; //mutation
	COnePointOptGlobal OPOG;//mutation
	CNearestNeighborRefinement NNR;
	CRandomNearestNeighborRefinement RNNR;
	CPartialNearestNeighborRefinement PNNR;
	CPartNN2Object PNN2O;
	CPartNEH2Object PNEH2O;
	CTwoOpt2Object OPT2O;
	CReDispatchRefinement RDR;
	CReArrangePermutation RAP;
	CNEHRefinement NEH;
	CRandomNEHRefinement RNEH;
	CPartialNEHRefinement PNEH;
	CFartoNearSort FNS;
	CTwoOpt OPT2;
	CAllPointOpt APO;
	SpeedOptimalAlgorithm SOA;
	SpeedOptimalAlgorithm2 SOA2;
	Initial_E IniE;


	//MyTimers.GetTimer("Gnuplot")->start();
	//Gnuplot gplot, gplot2; //�S���n�δN��mark��...�ܦ��ɶ�
	//MyTimers.GetTimer("Gnuplot")->end();

	int cur = 0, next = 1;
	//MyTimers.GetTimer("RandomIni")->start();
	RandomInitialization(&pop[cur], problem);
	//MyTimers.GetTimer("RandomIni")->end();
	double cnt = 0;
	int retry_num = 0, total_evaluate = 0;
	cout << "new" << endl;
	for (size_t i = 0; i < PopSize; i += 1)
	{
		/*if (problem.EvaluateDpCar(&pop[cur][i]))
		{
		cnt++;
		}*/
		//MyTimers.GetTimer("DPEvaluate")->start();
		//pop[cur][i].ShowRoute();
		if (problem.PRPDP(&pop[cur][i],50))//100�N��100%�ζZ����0%��emission��
		{
			pop[cur][i].e_up().resize(pop[cur][i].routes().size());
			pop[cur][i].e_down().resize(pop[cur][i].routes().size());
			size_t s = 0, e = 0;
			for (int v = 1; v < pop[cur][i].routes().size(); v++)
			{
				if (pop[cur][i].routes()[v] == problem.depot())
				{
					//cout << "\nv = " << v << endl;
					e = v;
					IniE(&pop[cur][i], problem, s, e);
					SOA2(&pop[cur][i], problem, s, e, e); // �@���@�x��(�@�����u)
					s = e; // ���U�@��route
				}
			}
			cnt += problem.EvaluateOldEncoding(&pop[cur][i]);
			
		}
		// --- pure eva ---- up
		//if (problem.EvaluateOldEncoding(&pop[cur][i]))//pure evaluate
		//{
		//	//cout << "in SOA" << endl;
		//	pop[cur][i].e_up().resize(pop[cur][i].routes().size());
		//	pop[cur][i].e_down().resize(pop[cur][i].routes().size());
		//	size_t s = 0, e = 0;
		//	for (int v = 1; v < pop[cur][i].routes().size(); v++)
		//	{
		//		if (pop[cur][i].routes()[v] == problem.depot())
		//		{
		//			//cout << "\nv = " << v << endl;
		//			e = v; 
		//			//IniE(&pop[cur][i], problem, s, e);
		//			//SOA(&pop[cur][i], problem, s, e, e); // �@���@�x��(�@�����u)
		//			s = e; // ���U�@��route
		//		}
		//	}
		//	
		//	//cout << "out SOA" << endl;
		//	cnt++;
		//	//cout << "EVAAAA " << endl;

		//}
		// --- pure eva ---- down
		total_evaluate++;
		//cout << "total eva = " <<total_evaluate << endl;
		//MyTimers.GetTimer("DPEvaluate")->end();

	}
	cout << "Initial pop feasible% = " << (cnt / PopSize) * 100 << '%' << endl; //getchar();

	double G_Min_fc = pop[cur][0].objs()[0];
	for (size_t t = 0; t<gen_num_; t += 1)
	{
		//cout << "I can't in here!" << endl; //random set exp and can't use refinement
		//getchar();
		fstream genfile;
		pop[0].Sort_pop(); //sort because I want to decide the weight 
		genfile.open(gen_record, ios::app);
		genfile << "gen = " << t + 1 << endl;
		//cout << "gen = " << t + 1 << endl; //getchar();
		cnt = 0;
		/*for (size_t i = 0; i < pop[cur].size(); i += 1){
		cout << pop[cur][i].objs()[0] << ' ' << pop[cur][i].objs()[1] << ' ' << pop[cur][i].num_vehicles() << endl;
		}*/
		//bool mu_and_refinement = false;

		/*�ˬdparent���h�֬O�@�˪�and ��XMax �M Min(Normalize)--------------------------*/
		vector<double> same_dis, same_emission;
		size_t infeasible_num = 0;
		double Max_fc = pop[cur][0].objs()[0], Min_fc = pop[cur][0].objs()[0], Max_emi = pop[cur][0].objs()[1], Min_emi = pop[cur][0].objs()[1];
		//fc = fuel consumption
		for (size_t p = 0; p < PopSize; p++)
		{
			//printf("pop[%d]_dis = %f\n", p, pop[cur][p].objs()[0]);
			//printf("pop[%d]_emi = %f\n", p, pop[cur][p].objs()[1]);
			if (pop[cur][p].objs()[0] >= 1e8) {
				infeasible_num++;
				continue; //���X�k�N���O�F
			}

			bool same = false;
			if (pop[cur][p].objs()[0] > Max_fc)
			{
				Max_fc = pop[cur][p].objs()[0];
			}
			if (pop[cur][p].objs()[0] < Min_fc)
			{
				Min_fc = pop[cur][p].objs()[0];
			}
			if (pop[cur][p].objs()[1] > Max_emi)
			{
				Max_emi = pop[cur][p].objs()[1];
			}
			if (pop[cur][p].objs()[1] < Min_emi)
			{
				Min_emi = pop[cur][p].objs()[1];
			}
			for (size_t j = 0; j < same_dis.size(); j++)
			{
				if (same_dis[j] == pop[cur][p].objs()[0] && same_emission[j] == pop[cur][p].objs()[1])
				{
					same = true;
					break;
				}
			}
			if (!same)
			{
				same_dis.push_back(pop[cur][p].objs()[0]);
				same_emission.push_back(pop[cur][p].objs()[1]);
			}
		}
		/*cout << "��" << same_dis.size() << "�ظ�" << endl;
		cout << "Max dis = " << Max_fc << endl;
		cout << "Min dis = " << Min_fc << endl;*/
		genfile << "��" << same_dis.size() << "�ظ�" << endl;
		genfile << "infeasible = " << infeasible_num << endl;
		genfile << "Max dis = " << Max_fc << endl;
		genfile << "Min dis = " << Min_fc << endl;
		genfile << "Max emi = " << Max_emi << endl;
		genfile << "Min emi = " << Min_emi << endl;
		if (Min_fc < G_Min_fc)
		{
			genfile << t + 1 << "�N��s�̨� " << Min_fc << endl;
			G_Min_fc = Min_fc;
			//cout << t + 1 << "�N��s�̨� " << Min_fc << endl;
		}
		double normal_dis = Max_fc - Min_fc, normal_emi = Max_emi - Min_emi;
		if (normal_dis == 0) normal_dis = 1;
		if (normal_emi == 0) normal_emi = 1;
		genfile << endl;
		genfile.close();
		//-----------------------------------------------------

		pop[cur].resize(PopSize * 2); //���@�bSize�n���Ӹ�child
		for (size_t i = 0; i < PopSize; i += 2)
		{
			int father = rand() % PopSize,
				mother = rand() % PopSize;
			//cout << "rand OK" << endl;
			//�}�l�L����parent
			//cout << "parent -- " << endl;
			/*WriteFile(pop[cur][father], file_name);
			WriteFile(pop[cur][mother], file_name);*/
			//---------------------------------------------------------------------------------------

			//crossover function
			//OX(&pop[cur][PopSize+i], &pop[cur][PopSize+i+1], pop[cur][father], pop[cur][mother]);
			LOX(&pop[cur][PopSize + i], &pop[cur][PopSize + i + 1], pop[cur][father], pop[cur][mother]);
			//cout << "crossover OK" << endl;
			//bool mu1 = false, mu2 = false; //���nmutation �٬O�n��mu1,mu2
			//swap mutation
			//bool mu1 = swap_mutation(&pop[cur][PopSize + i]); //2point 
			//bool mu2 = swap_mutation(&pop[cur][PopSize + i + 1]);
			/*problem.EvaluateDpCar(&pop[cur][PopSize + i]);
			problem.EvaluateDpCar(&pop[cur][PopSize + i + 1]);*/
			//cout << "father = " << father << endl;
			//cout << "mother = " << mother << endl;

			double obj1_rate1 = 100 - (double)father / PopSize * 100;
			if (obj1_rate1 == 0)
			{
				obj1_rate1 = 1;
			}
			else if (obj1_rate1 == 100)
			{
				obj1_rate1 = 99;
			}
			double obj1_rate2 = 100 - (double)mother / PopSize * 100;
			if (obj1_rate2 == 0)
			{
				obj1_rate2 = 1;
			}
			else if (obj1_rate2 == 100)
			{
				obj1_rate2 = 99;
			}
			//cout << "obj1 = " << obj1_rate << endl;
			if (problem.PRPDP(&pop[cur][PopSize + i], obj1_rate1))//100�N��100%�ζZ����0%��emission��
			//if (problem.PRPDP(&pop[cur][PopSize + i],90))
			{
				//cout << "father" << endl;
				pop[cur][PopSize + i].e_up().resize(pop[cur][PopSize + i].routes().size());
				pop[cur][PopSize + i].e_down().resize(pop[cur][PopSize + i].routes().size());
				size_t s = 0, e = 0;
				for (int v = 1; v <pop[cur][PopSize + i].routes().size(); v++)
				{
					if (pop[cur][PopSize + i].routes()[v] == problem.depot())
					{
						//cout << "\nv = " << v << endl;
						e = v;
						IniE(&pop[cur][PopSize + i], problem, s, e);
						SOA2(&pop[cur][PopSize + i], problem, s, e, e); // �@���@�x��(�@�����u)
						s = e; // ���U�@��route
					}
				}

				//cnt++;
				//cout << "PRP OK1" << endl;
			}
			else
			{
				cout << "PRP false1" << endl; getchar();
			}
			//cnt += problem.PRPDP(&pop[cur][PopSize + i], obj1_rate);
			//cout << "after SOA1" << endl;
			cnt += problem.EvaluateOldEncoding(&pop[cur][PopSize + i]);

			if (problem.PRPDP(&pop[cur][PopSize + i + 1], obj1_rate2))//100�N��100%�ζZ����0%��emission��
			//if (problem.PRPDP(&pop[cur][PopSize + i + 1], 90))
			{
				//cout << "mother" << endl;
				pop[cur][PopSize + i + 1].e_up().resize(pop[cur][PopSize + i + 1].routes().size());
				pop[cur][PopSize + i + 1].e_down().resize(pop[cur][PopSize + i + 1].routes().size());
				size_t s = 0, e = 0;
				for (int v = 1; v < pop[cur][PopSize + i + 1].routes().size(); v++)
				{
					if (pop[cur][PopSize + i + 1].routes()[v] == problem.depot())
					{
						//cout << "\nv = " << v << endl;
						e = v;
						IniE(&pop[cur][PopSize + i + 1], problem, s, e);
						SOA2(&pop[cur][PopSize + i + 1], problem, s, e, e); // �@���@�x��(�@�����u)
						s = e; // ���U�@��route
					}
				}
				//cout << "PRP OK2" << endl;
				//cnt++;
			}
			else
			{
				cout << "PRP false2" << endl; getchar();
			}
			//cout << "after SOA2" << endl;
			cnt += problem.EvaluateOldEncoding(&pop[cur][PopSize + i + 1]);
			//cout << "obj2 = " << obj1_rate << endl;
			//cnt += problem.PRPDP(&pop[cur][PopSize + i + 1], obj1_rate);
			//if (mother == 99) getchar();
			//problem.Dp2Object(&pop[cur][PopSize + i], 100);
			//problem.Dp2Object(&pop[cur][PopSize + i + 1], 100);
			total_evaluate += 4;
			//cout << "PRP OK" << endl;
			//�}�l�L�u�@crossover �M DP��child-----------------------------
			/*WriteFile(pop[cur][PopSize + i], file_name);
			WriteFile(pop[cur][PopSize + i + 1], file_name);*/
			//----------------------------------------------------------------------------

			//two point mutation
			//�S�b�o��mutation �N�����Droutes

			//bool mu1 = two_point_mutation(&pop[cur][PopSize + i]);
			//bool mu2 = two_point_mutation(&pop[cur][PopSize + i + 1]);
			const int num_refinement = 3;
			size_t num1 = rand() % num_refinement, num2 = rand() % num_refinement;
			const size_t NN_index = 0, NEH_index = 1, OPT2_index = 2;

			/*if (mu1) {
			OPOG(&pop[cur][PopSize + i], problem);
			}
			if (mu2) {
			OPOG(&pop[cur][PopSize + i + 1], problem);
			}*/
			/*if (mu1) {
			cout << "after mutation 1:" << endl;
			problem.EvaluateOldEncoding(&pop[cur][PopSize + i]);
			WriteFile(pop[cur][PopSize + i], file_name);
			WriteFile(pop[cur][PopSize + i + 1], file_name);
			}
			if (mu2) {
			cout << "after mutation 2:" << endl;
			problem.EvaluateOldEncoding(&pop[cur][PopSize + i + 1]);
			WriteFile(pop[cur][PopSize + i], file_name);
			WriteFile(pop[cur][PopSize + i + 1], file_name);
			}*/

			//cout << "after ReDispatchRefinement:" << endl;
			//if (!mu1 || mu_and_refinement) {
			//	RDR(&pop[cur][PopSize + i], problem);
			//	/*problem.EvaluateOldEncoding(&pop[cur][PopSize + i]);
			//	WriteFile(pop[cur][PopSize + i], file_name); 
			//	WriteFile(pop[cur][PopSize + i + 1], file_name);*/
			//}
			//if (!mu2 || mu_and_refinement) {
			//	RDR(&pop[cur][PopSize + i + 1], problem);
			//	/*problem.EvaluateOldEncoding(&pop[cur][PopSize + i + 1]);
			//	WriteFile(pop[cur][PopSize + i], file_name);
			//	WriteFile(pop[cur][PopSize + i + 1], file_name);*/
			//}
			//cout << "after NearestNeighborRefinement : " << endl;
			//if (num1 == NN_index) {
			//	//if (t < 33) {
			//	//	NNR(&pop[cur][PopSize + i], problem);
			//	//	RNNR(&pop[cur][PopSize + i], problem);
			//	PNNR(&pop[cur][PopSize + i], problem);
			//	//PNN2O(&pop[cur][PopSize + i], problem, PopSize - father, normal_dis, normal_emi);
			//	//PNN2O(&pop[cur][PopSize + i], problem,100, normal_dis, normal_emi);
			//	//total_evaluate += pop[cur][PopSize + i].num_evaluate(); //NN�n������
			//	problem.EvaluateOldEncoding(&pop[cur][PopSize + i]);
			//		//WriteFile(pop[cur][PopSize + i], file_name);
			//		//WriteFile(pop[cur][PopSize + i + 1], file_name);
			//}
			//if (num2 == NN_index) {
			//	//if (t < 33) {
			//	//	NNR(&pop[cur][PopSize + i + 1], problem);
			//	//	RNNR(&pop[cur][PopSize + i + 1], problem);
			//	PNNR(&pop[cur][PopSize + i + 1], problem);
			//	//PNN2O(&pop[cur][PopSize + i + 1], problem, PopSize - mother, normal_dis, normal_emi);
			//	//total_evaluate += pop[cur][PopSize + i + 1].num_evaluate();//NN�n������
			//	//PNN2O(&pop[cur][PopSize + i + 1], problem, 100, normal_dis, normal_emi);
			//	problem.EvaluateOldEncoding(&pop[cur][PopSize + i + 1]);
			//		//WriteFile(pop[cur][PopSize + i], file_name);
			//		//WriteFile(pop[cur][PopSize + i + 1], file_name);
			//}

			//cout << "after FartoNearSort : " << endl;
			//if (!mu1 || mu_and_refinement){
			//	FNS(&pop[cur][PopSize + i], problem);
			//	/*problem.EvaluateOldEncoding(&pop[cur][PopSize + i]);
			//	WriteFile(pop[cur][PopSize + i], file_name);
			//	WriteFile(pop[cur][PopSize + i + 1], file_name);*/
			//}
			//if (!mu2 || mu_and_refinement) {
			//	FNS(&pop[cur][PopSize + i + 1], problem);
			//	/*problem.EvaluateOldEncoding(&pop[cur][PopSize + i + 1]);
			//	WriteFile(pop[cur][PopSize + i], file_name);
			//	WriteFile(pop[cur][PopSize + i + 1], file_name);*/
			//}

			//cout << "after NEH : " << endl;
			if (num1 == NEH_index) {
				//if(t >=33 && t < 66){
				//	NEH(&pop[cur][PopSize + i], problem);
				//	RNEH(&pop[cur][PopSize + i], problem);
				PNEH(&pop[cur][PopSize + i], problem);
				//PNEH2O(&pop[cur][PopSize + i], problem, PopSize - father, normal_dis, normal_emi);
				//total_evaluate += pop[cur][PopSize + i].num_evaluate();//NEH�]����?
				//PNEH2O(&pop[cur][PopSize + i], problem, 100, normal_dis, normal_emi);
				//problem.EvaluateOldEncoding(&pop[cur][PopSize + i]);
				//problem.PRPDP(&pop[cur][PopSize + i], obj1_rate1);
				/*WriteFile(pop[cur][PopSize + i], file_name);
				WriteFile(pop[cur][PopSize + i + 1], file_name);*/
			}
			if (num2 == NEH_index) {
				//if (t >= 33 && t < 66) {
				//	NEH(&pop[cur][PopSize + i + 1], problem);
				//	RNEH(&pop[cur][PopSize + i + 1], problem);
				PNEH(&pop[cur][PopSize + i + 1], problem);
				//PNEH2O(&pop[cur][PopSize + i + 1], problem, PopSize - mother, normal_dis, normal_emi);
				//total_evaluate += pop[cur][PopSize + i + 1].num_evaluate();//NEH�]����?
				//PNEH2O(&pop[cur][PopSize + i + 1], problem, 100, normal_dis, normal_emi);
				//problem.PRPDP(&pop[cur][PopSize + i + 1], obj1_rate2);
				
				//problem.EvaluateOldEncoding(&pop[cur][PopSize + i + 1]);
				/*WriteFile(pop[cur][PopSize + i], file_name);
				WriteFile(pop[cur][PopSize + i + 1], file_name);*/
			}
			//cout << "After OPO mutation" << endl; //�o��mutation�n��b�᭱���������
			/*if (mu1) {
			OPO(&pop[cur][PopSize + i], problem);
			}
			if (mu2) {
			OPO(&pop[cur][PopSize + i + 1], problem);
			}*/

			//2-OPT
			//cout << "After 2-OPT" << endl;
			if (num1 == OPT2_index) { // t �ӬݲĴX�N�n��mutation
				OPT2(&pop[cur][PopSize + i], problem);
				//problem.PRPDP(&pop[cur][PopSize + i], obj1_rate1);
				//problem.EvaluateOldEncoding(&pop[cur][PopSize + i]);

				//if (t >= 66) {
				//	
				//	OPT2O(&pop[cur][PopSize + i], problem, PopSize - father, normal_dis, normal_emi, Min_fc, Min_emi);
				//	total_evaluate += pop[cur][PopSize + i].num_evaluate();
				//}
				//bool progress = true;
				//while (progress)
				//{
				//	progress = false;
				//	double old = pop[cur][PopSize + i].objs()[0];
				//	OPT2O(&pop[cur][PopSize + i], problem, 100 - father, normal_dis, normal_emi, Min_fc, Min_emi);
				//	//problem.Dp2Object(&pop[cur][PopSize + i], 100 - father);
				//	total_evaluate += pop[cur][PopSize + i].num_evaluate() + 1;
				//	if (pop[cur][PopSize + i].objs()[0] < old) progress = true;

				//}

				//OPT2O(&pop[cur][PopSize + i], problem, 100, normal_dis, normal_emi, Min_fc, Min_emi);
			}
			if (num2 == OPT2_index) {
				//if (t >= 66) {
				OPT2(&pop[cur][PopSize + i + 1], problem);
				//problem.PRPDP(&pop[cur][PopSize + i + 1], obj1_rate2);
				//problem.EvaluateOldEncoding(&pop[cur][PopSize + i + 1]);
				/*OPT2O(&pop[cur][PopSize + i + 1], problem, PopSize - mother, normal_dis, normal_emi, Min_fc, Min_emi);
				total_evaluate += pop[cur][PopSize + i + 1].num_evaluate();*/
				/*bool progress = true;
				while (progress)
				{
				progress = false;
				double old = pop[cur][PopSize + i + 1].objs()[0];
				OPT2O(&pop[cur][PopSize + i + 1], problem, 100 - mother, normal_dis, normal_emi, Min_fc, Min_emi);
				problem.Dp2Object(&pop[cur][PopSize + i + 1], 100 - mother);
				total_evaluate += pop[cur][PopSize + i + 1].num_evaluate() + 1;
				if (pop[cur][PopSize + i + 1].objs()[0] < old) progress = true;
				}*/

				//OPT2O(&pop[cur][PopSize + i + 1], problem, 100, normal_dis, normal_emi, Min_fc, Min_emi);
			}
			//One Point Optimal Global
			//cout << "after One point optimal (Global)" << endl;
			//if (num1 == 3 && t <= 100) { // t �ӬݲĴX�N�n��mutation
			//	OPOG(&pop[cur][PopSize + i], problem);
			//}
			//if (num2 == 3 && t <= 100) {
			//	OPOG(&pop[cur][PopSize + i + 1], problem);
			//}
			/*cout << "before RAP" << endl;
			cout << pop[cur][PopSize + i] << endl;
			cout << pop[cur][PopSize + i + 1] << endl;
			
			pop[cur][PopSize + i].ShowRoute();
			pop[cur][PopSize + i + 1].ShowRoute();*/
			//cout << "after ReArrangeMent : " << endl; //��routes���Ʀ^indv->vars()
			if (num1 != 0)
			{
				RAP(&pop[cur][PopSize + i], problem);
				problem.PRPDP(&pop[cur][PopSize + i], obj1_rate1);
			}
			if (num2 != 0)
			{
				RAP(&pop[cur][PopSize + i + 1], problem);
				problem.PRPDP(&pop[cur][PopSize + i + 1], obj1_rate2);
			}
			
			/*cout << "After RAP" << endl;
			cout << pop[cur][PopSize + i] << endl;
			cout << pop[cur][PopSize + i + 1] << endl; getchar();*/

			//cnt += problem.EvaluateOldEncoding(&pop[cur][PopSize + i]);//pure evaluate
			//cnt += problem.EvaluateOldEncoding(&pop[cur][PopSize + i + 1]);
			/*cnt += problem.EvaluateDpCar(&pop[cur][PopSize + i]);
			cnt += problem.EvaluateDpCar(&pop[cur][PopSize + i + 1]);*/

			/*cnt += problem.Dp2Object(&pop[cur][PopSize + i], PopSize - father);
			cnt += problem.Dp2Object(&pop[cur][PopSize + i + 1], PopSize - mother);
			total_evaluate += 2;*/

			//cnt += problem.Dp2Object(&pop[cur][PopSize + i], 100);
			//cnt += problem.Dp2Object(&pop[cur][PopSize + i + 1], 100);
			//system("pause");
			//if (pop[cur][PopSize + i].objs()[0] - 5 <= Min_fc && t<0) //t is num of gen �{�b�O�ĴX�N
			//{
			//	APO(&pop[cur][PopSize + i], problem, PopSize - father, normal_dis, normal_emi, Min_fc, Min_emi);
			//	RAP(&pop[cur][PopSize + i], problem);
			//	problem.Dp2Object(&pop[cur][PopSize + i], PopSize - father);
			//	total_evaluate += pop[cur][PopSize + i].num_evaluate() + 1;
			//}
			//if (pop[cur][PopSize + i + 1].objs()[0] - 5 <= Min_fc && t<0)
			//{
			//	APO(&pop[cur][PopSize + i + 1], problem, PopSize - mother, normal_dis, normal_emi, Min_fc, Min_emi);
			//	RAP(&pop[cur][PopSize + i + 1], problem);
			//	problem.Dp2Object(&pop[cur][PopSize + i + 1], PopSize - mother);
			//	total_evaluate += pop[cur][PopSize + i + 1].num_evaluate() + 1;
			//}
			//�L�����Ҧ��ާ@��child-----------------------------
			/*WriteFile(pop[cur][PopSize + i], file_name);
			WriteFile(pop[cur][PopSize + i + 1], file_name);*/
			//----------------------------------
			//system("pause");

			//�˹�򤧫e���L�ۦP��chrome 
			int num_mu = 2;
			//for (int j = 0; j < PopSize + i; j++) //father and child
			//{
			//	if (Same(pop[cur][PopSize + i], pop[cur][j]))
			//	{
			//		bool change_OK = false;
			//		//size_t change_num = 0;
			//		//cout << "I want change!!" << endl;
			//		while (!change_OK)
			//		{
			//			int rand_mu = rand() % num_mu;
			//			retry_num++;
			//			if (rand_mu == 0) {
			//				OPO(&pop[cur][PopSize + i], problem);
			//				total_evaluate += pop[cur][PopSize + i].num_evaluate();
			//			}
			//			else if (rand_mu == 1) {
			//				two_point_mutation(&pop[cur][PopSize + i]);
			//				total_evaluate += 1;
			//			}
			//			RAP(&pop[cur][PopSize + i], problem);
			//			problem.Dp2Object(&pop[cur][PopSize + i], PopSize - father);
			//			total_evaluate += 1;
			//			//change_num++;
			//			if (!Same(pop[cur][PopSize + i], pop[cur][j]))
			//			{
			//				change_OK = true;
			//			}
			//		}
			//		//cout << "change num = " << change_num << endl;
			//		//getchar();
			//		j = -1;
			//	}
			//}

			//for (int j = 0; j < PopSize + i + 1; j++) //mother and child
			//{
			//	if (Same(pop[cur][PopSize + i + 1], pop[cur][j]))
			//	{
			//		bool change_OK = false;
			//		//size_t change_num = 0;
			//		//cout << "I want change!!" << endl;
			//		while (!change_OK)
			//		{
			//			retry_num++;
			//			int rand_mu = rand() % num_mu;
			//			//int mu_num = 1;
			//			if (rand_mu == 0) {
			//				OPO(&pop[cur][PopSize + i + 1], problem);
			//				total_evaluate += pop[cur][PopSize + i + 1].num_evaluate();
			//			}
			//			else if (rand_mu == 1) {
			//				two_point_mutation(&pop[cur][PopSize + i + 1]);
			//				total_evaluate += 1;
			//			}

			//			RAP(&pop[cur][PopSize + i + 1], problem);
			//			problem.Dp2Object(&pop[cur][PopSize + i + 1], 100 - mother);
			//			total_evaluate += 1;
			//			if (!Same(pop[cur][PopSize + i + 1], pop[cur][j]))
			//			{
			//				change_OK = true;
			//			}
			//		}
			//		//cout << "change num = " << change_num << endl;
			//		//getchar();
			//		j = -1;
			//	}
			//}
			//cout << t << '_' << i << " not finish~ " << endl;
			//CheckSame_pop(pop[cur]);
			//cout << "reproduction " << i << endl;
			/*ShowChange(pop[cur][father], pop[cur][PopSize + i]);
			ShowChange(pop[cur][mother], pop[cur][PopSize + i + 1]);*/
			//Sleep(50);
		}
		cout << "child feasible = " << (double)cnt / PopSize * 100 << '%' << endl;
		// --- check if duplicate
		//CheckSame_pop(pop[cur]);
		// --- draw plot function
		//ShowPopulation(gplot2, pop[cur], "pop-before"); Sleep(250); 
		EnvironmentalSelection(&pop[next], &pop[cur], rps, PopSize);
		//cout << "gen = " << t + 1 << endl;
		//ShowPopulation(gplot, pop[next], "pop"); Sleep(100); //system("pause");

		std::swap(cur, next);
	}
	// --- show the retry and total eva num
	//cout << "retry = " << retry_num << endl;
	//cout << "total eva = " << total_evaluate << endl;

	// --- write eva in file
	fstream eva_file;
	eva_file.open(file_name, ios::app);
	if (!eva_file)cout << "Can't write eva..." << endl;
	eva_file << "total eva = " << total_evaluate << endl;
	eva_file.close();
	//getchar();

	// --- change to new pop
	*solutions = pop[cur];
	//MyTimers.GetTimer("AllSolve")->end();
}


void CNSGAIII::WriteFile(CIndividual chromosome, string file_name)
{
	/*
	��chrome ����T�g���ɮ׸�
	*/
	fstream file;
	file.open(file_name, ios::app);
	if (!file) cout << "Can't open " << file_name << endl;
	//�}�l�L����Cromosome data-----------------------------
	cout << "c1 dis = " << chromosome.objs()[0] << endl;

	file << "x chrome : ";
	for (auto e : chromosome.routes())
	{
		file << e << " ";
	}
	file << endl;
	file << "total distance = " << chromosome.objs()[0] << endl;
	file << "emission = " << chromosome.objs()[1] << endl;
	file.close();
}

bool CNSGAIII::Same(const CIndividual ch1, const CIndividual ch2)
{
	// --- Return true, if two chromesome's objective values are the equal 
	if (ch1.objs()[0] == ch2.objs()[0] && ch1.objs()[1] == ch2.objs()[1])
	{
		return true;
	}
	return false;
}

void CNSGAIII::ShowChange(const CIndividual ch1, const CIndividual ch2)
{
	printf("( %.2f + %.2f ) -> ( %.2f + %.2f )\n", ch1.objs()[0], ch1.objs()[1], ch2.objs()[0], ch2.objs()[1]);
}

bool CNSGAIII::CheckSame_pop(const CPopulation p)
{
	bool flag = false;
	for (int i = 0; i < p.size(); i++)
	{
		if (p[i].objs()[0] == 0) continue;
		for (int j = i + 1; j < p.size(); j++)
		{
			if (p[i].objs()[0] == p[j].objs()[0] && p[i].objs()[1] == p[j].objs()[1])
			{
				cout << i << " = " << j << endl;
				cout << p[i].objs()[0] << ' ' << p[i].objs()[1] << endl;
				flag = true;
			}
		}
	}
	cout << "Finish Check" << endl;
	if (flag) getchar();
	return flag;
}