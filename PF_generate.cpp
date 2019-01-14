#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

struct Point
{
    double x,y;

    bool operator < (const Point & tmp) const
    {
        if(x == tmp.x)
        {

            return y < tmp.y;
        }
        return x < tmp.x;
    }
};

int main()
{
    ifstream exp("explist.ini");
    if(!exp) { cout << "We need the explist.ini file." << endl; return 1; }
    string exp_name ;
    while(exp >> exp_name)
    {
        exp_name.resize(exp_name.size()-4);
        cout << exp_name << endl;
        ifstream point_file("Point\\" + exp_name +"-point.txt");
        if(!point_file){cout << "no point file." << endl; return 1;}

        //vector<double> x,y;
        vector<Point> p;

        //double tmpx,tmpy;
        Point tmp;
        int cnt = 0;
        while(point_file >> tmp.x >> tmp.y)
        {
            bool del = false;
            for(int i=0;i<p.size();i++)
            {
                if(p[i].x < tmp.x && p[i].y < tmp.y)
                {
                    del = true;
                    break;
                }
            }
            if(!del)
            {
                p.push_back(tmp);
                //x.push_back(tmpx);
                //y.push_back(tmpy);
            }
            cnt ++;
        }
        cout << "cnt = " << cnt << endl;
        const double outvalue = 1e9;
        bool del = true;
        while(del)
        {
            del = false;
            for(int i=0;i<p.size();i++)
            {
                if(p[i].x == outvalue) continue;
                for(int j=0;j<p.size();j++)
                {
                    if(i==j || p[j].x== outvalue) continue;
                    if(p[i].x < p[j].x && p[i].y < p[j].y)
                    {
                       p[j].x = outvalue;
                       del = true;
                    }
                    else if(p[i].x == p[j].x && p[i].y < p[j].y)
                    {
                        p[j].x = outvalue;
                        del = true;
                    }
                    else if(p[i].x < p[j].x && p[i].y == p[j].y)
                    {
                        p[j].x = outvalue;
                        del = true;
                    }
                    if(p[i].x == p[j].x && p[i].y == p[j].y)
                    {
                        p[j].x = outvalue;
                        del = true;
                    }
                }
            }
        }
        //p.resize(p.size());
        sort(p.begin(),p.end());
        //cout << "stop" << endl; getchar();
        fstream out_file;
        out_file.open("PF\\"+ exp_name + "-PF.txt",ios::out);
        if(!out_file){cout << "You can't open file."<<endl; return 1;}
        cout << "psize = " << p.size() << endl;
        int final_cnt = 0;
        for(int i=0;i<p.size();i++)
        {
            if(p[i].x == outvalue)
            {
                break;

            }
            out_file << p[i].x << ' ' << p[i].y << endl;
            //cout << p[i].x << ' ' << p[i].y << endl;
            final_cnt ++;
        }
        cout << "final = " << final_cnt << endl;
        //getchar();
    }

    return 0;
}

