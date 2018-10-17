/*
    大量製造.ini的程式
    可以去操控要改變哪個變數
    要去讀list.txt
    裡面要有每個要改的檔名.txt
    會生出他的.ini
*/
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main()
{
    int project_num = 9;
    string project[20];
    project[0] = "algorithm.name = ";
    project[1] = "objective.division.p = ";
    project[2] = "generation.number = ";
    project[3] = "crossover.rate = ";
    project[4] = "crossover.eta = ";
    project[5] = "mutation.eta = ";
    project[6] = "problem.name = ";
    project[7] = "problem.variables = ";
    project[8] = "problem.objectives = ";

    string ans[20],file_name;
    ans[0] = "NSGAIII"; //alg_name = "NSGAIII",
    ans[1] = "999"; //obj_d_p = "999",
    ans[2] = "0"; //gen_num = "100",
    ans[3] = "1.0"; //cro_rate = "1.0",
    ans[4] = "30"; // cro_eta = "30",
    ans[5] = "20"; //mu_eta = "20",
    //ans[6] = "UK10_01"; //p_name = "UK10_01", //要調整這個
    ans[7] = "102"; //p_var = "102",
    ans[8] = "2";  //p_obj = "2",

    fstream readfile;

    readfile.open("list.txt",ios::in);
    if(!readfile)
    {
        cout << "list.txt does not exist!" << endl;
        getchar();
        return -1;
    }

    while(readfile >> file_name)
    {
        file_name.resize(file_name.size()-4);
        ans[6] = file_name;
        file_name += ".ini";
        fstream outfile(file_name,ios::out);

        for(int i=0;i<project_num;i++)
        {
            outfile << project[i] << ans[i];
            if(i != project_num-1) outfile << endl;
        }
        outfile.close();
    }
    readfile.close();

    return 0;
}
