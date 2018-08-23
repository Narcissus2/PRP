#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main()
{
    fstream outfile;
    string filename = "test.ini";
    outfile.open(filename, ios::out | ios::app);

    if (!outfile) {
		cout << "Can't open " << filename << endl;
		getchar();
		return false;
	}

	string f = "UK",instance = "200_",b = ".ini";
	int num = 20;
	for(int i=1;i<=num;i++)
    {
        string n,n2;
        n = (i/10)+'0';
        n2 = (i%10) + '0';
        outfile << f << instance << n << n2 << b << endl;
    }

    outfile.close();

    return 0;
}
