
#include "alg_crossover.h"
#include "alg_individual.h"
#include "aux_math.h"
#include "problem_base.h"

#include <cmath>
#include <algorithm>
#include <set>
#include <cstddef>
#include <stdlib.h>

using std::size_t;

// ----------------------------------------------------------------------
// The implementation was adapted from the code of function realcross() in crossover.c
// http://www.iitk.ac.in/kangal/codes/nsga2/nsga2-gnuplot-v1.1.6.tar.gz
//
// ref: http://www.slideshare.net/paskorn/simulated-binary-crossover-presentation#
// ----------------------------------------------------------------------
double CSimulatedBinaryCrossover::get_betaq(double rand, double alpha, double eta) const
{
    double betaq = 0.0;
    if (rand <= (1.0/alpha))
    {
        betaq = std::pow((rand*alpha),(1.0/(eta+1.0)));
    }
    else
    {
        betaq = std::pow((1.0/(2.0 - rand*alpha)),(1.0/(eta+1.0)));
    }
    return betaq;
}
// ----------------------------------------------------------------------
//這用不到先mark-------------------------------------------------------
//bool CSimulatedBinaryCrossover::operator()(CIndividual *child1,
//        CIndividual *child2,
//        const CIndividual &parent1,
//        const CIndividual &parent2,
//        double cr,
//        double eta) const
//{
//    *child1 = parent1;
//    *child2 = parent2;
//
//    if (MathAux::random(0.0, 1.0) > cr) return false; // not crossovered
//
//    CIndividual::TDecVec &c1 = child1->vars(), &c2 = child2->vars();
//    const CIndividual::TDecVec &p1 = parent1.vars(), &p2 = parent2.vars();
//
//    for (size_t i=0; i<c1.size(); i+=1)
//    {
//        if (MathAux::random(0.0, 1.0) > 0.5) continue; // these two variables are not crossovered
//        if (std::fabs(p1[i]-p2[i]) <= MathAux::EPS) continue; // two values are the same
//
//        double y1 = std::min(p1[i], p2[i]),
//               y2 = std::max(p1[i], p2[i]);
//
//        CIndividual::TGene lb = CIndividual::TargetProblem().lower_bounds()[i],
//                           ub = CIndividual::TargetProblem().upper_bounds()[i];
//
//        double rand = MathAux::random(0.0, 1.0);
//
//        // child 1
//        double beta = 1.0 + (2.0*(y1-lb)/(y2-y1)),
//               alpha = 2.0 - std::pow(beta, -(eta+1.0));
//        double betaq = get_betaq(rand, alpha, eta);
//
//        c1[i] = 0.5*((y1+y2)-betaq*(y2-y1));
//
//        // child 2
//        beta = 1.0 + (2.0*(ub-y2)/(y2-y1));
//        alpha = 2.0 - std::pow(beta, -(eta+1.0));
//        betaq = get_betaq(rand, alpha, eta);
//
//        c2[i] = 0.5*((y1+y2)+betaq*(y2-y1));
//
//        // boundary checking
//        c1[i] = std::min(ub, std::max(lb, c1[i]));
//        c2[i] = std::min(ub, std::max(lb, c2[i]));
//
//        if (MathAux::random(0.0, 1.0) <= 0.5)
//        {
//            std::swap(c1[i], c2[i]);
//        }
//    }
//
//    return true;
//}// CSimulatedBinaryCrossover


//--------------------------------------------------------------------------------------
bool CPartiallyMappedCrossover::operator()(CIndividual *child1,
        CIndividual *child2,
        const CIndividual &parent1,
        const CIndividual &parent2,
        double cr) const
{
    *child1 = parent1;
    *child2 = parent2;
    if (MathAux::random(0.0, 1.0) > cr) return false; // not crossovered

    CIndividual::TDecVec &c1 = child1->vars(), &c2 = child2->vars();
    const CIndividual::TDecVec &p1 = parent1.vars(), &p2 = parent2.vars();
    CIndividual::TDecVec c1ndepot, c2ndepot, c1map, c2map;
    std::vector<int> c1rlen, c2rlen;
    //Get the sequence without depot
    CIndividual::TGene depot = c1[0];
    for (size_t i=1,cnt=0 ; i<c1.size() ; ++i)
    {
        if(c1[i] == depot)  c1rlen.push_back(cnt);
        else c1ndepot.push_back(c1[i]);
        cnt = (c1[i] == depot) ? 0 : cnt+1;
    }
    for (size_t i=1,cnt=0 ; i<c2.size() ; ++i)
    {
        if(c2[i] == depot)  c2rlen.push_back(cnt);
        else c2ndepot.push_back(c2[i]);
        cnt = (c2[i] == depot) ? 0 : cnt+1;
    }
    if(c1ndepot.size() == 0 || c2ndepot.size() == 0) return false;

    size_t srt=0, end=0;
    std::vector<CIndividual::TGene>::iterator tdvp;
    CIndividual::TGene target;
    //child1
    //random one routine to reserve.
    size_t select = rand()%c1rlen.size();
    for(size_t i=0 ; i<select ; ++i)
    {
        srt += c1rlen[i];
    }
    end = srt+c1rlen[select]-1;

    //set the reserve one and mapping.
    c1map.resize(c1ndepot.size(), 0);
    for(size_t i=srt ; i<=end; ++i)
    {
        c1map[i] = c1ndepot[i];
        target = c1ndepot[i];
        if(std::find(c1ndepot.begin()+srt, c1ndepot.begin()+end+1, c2ndepot[i]) == c1ndepot.begin()+end+1)
        {
            while(true)
            {
                tdvp = std::find(c2ndepot.begin(), c2ndepot.end(), target);
                size_t pos = tdvp-c2ndepot.begin();
                if(pos<srt || pos>end)
                {
                    c1map[pos] = c2ndepot[i];
                    break;
                }
                target = c1ndepot[pos];
            }
        }
    }

    //fill the spot which is still empty.
    for(size_t i=0 ; i<c1map.size() ; ++i)
    {
        if(c1map[i] == 0) c1map[i] = c2ndepot[i];
    }

    for(size_t i=0, j=0 ; i<c1.size() ; ++i)
    {
        if(c1[i] == depot) continue;
        c1[i] = c1map[j];
        j+=1;
    }

    //child2
    srt = 0;
    select = rand()%c2rlen.size();
    for(size_t i=0 ; i<select ; ++i)
    {
        srt += c2rlen[i];
    }
    end = srt+c2rlen[select]-1;

    c2map.resize(c2ndepot.size(), 0);
    for(size_t i=srt ; i<=end; ++i)
    {
        c2map[i] = c2ndepot[i];
        target = c2ndepot[i];
        if(std::find(c2ndepot.begin()+srt, c2ndepot.begin()+end+1, c1ndepot[i]) == c2ndepot.begin()+end+1)
        {
            while(true)
            {
                tdvp = std::find(c1ndepot.begin(), c1ndepot.end(), target);
                size_t pos = tdvp-c1ndepot.begin();
                if(pos<srt || pos>end)
                {
                    c2map[pos] = c1ndepot[i];
                    break;
                }
                target = c2ndepot[pos];
            }
        }
    }

    //fill the spot which is still empty.
    for(size_t i=0 ; i<c2map.size() ; ++i)
    {
        if(c2map[i] == 0) c2map[i] = c1ndepot[i];
    }

    for(size_t i=0, j=0 ; i<c2.size() ; ++i)
    {
        if(c2[i] == depot) continue;
        c2[i] = c2map[j];
        j+=1;
    }
    return true;

}// CPartiallyMappedCrossover


 // ----------------------------------------------------------------------------------
 //		COrderedCrossover : Ordered Crossover (OX)
 // ----------------------------------------------------------------------------------
bool COrderedCrossover::operator()(CIndividual *child1, CIndividual *child2, const CIndividual &parent1, const CIndividual &parent2, double cr) const
{
	const size_t Depot = parent1.vars()[0];

	if (MathAux::random(0.0, 1.0) > cr) return false; // not crossovered

	CIndividual::TDecVec *children[] = { &child1->vars(), &child2->vars() };
	const CIndividual::TDecVec *parents[] = { &parent1.vars(), &parent2.vars() };

	for (size_t c = 0; c < 2; c += 1)
	{
		size_t cut1 = rand() % children[c]->size(), cut2 = rand() % children[c]->size();
		if (cut1 > cut2) swap(cut1, cut2);

		size_t father = c;

		set<CIndividual::TGene> exists;
		for (size_t i = cut1; i <= cut2; i += 1) // keep the enclosed section
		{
			(*children[c])[i] = (*parents[father])[i];
			exists.insert((*children[c])[i]);
		}

		size_t c_pos = (cut2 + 1)%children[c]->size(), m_pos = c_pos;
		size_t mother = !c;

		while (c_pos != cut1)
		{
			while (exists.count((*parents[mother])[m_pos]) > 0) m_pos = (m_pos + 1) % parents[mother]->size();
		
			(*children[c])[c_pos] = (*parents[mother])[m_pos];
			exists.insert((*children[c])[c_pos]);

			c_pos = (c_pos + 1) % children[c]->size();
		}
		swap((*children[c])[0], *find(children[c]->begin(), children[c]->end(), Depot));
	}// for - children 0 and 1

	return true;

}// COrderedCrossover::operator()


 // ----------------------------------------------------------------------------------
 //		CLinearOrderedCrossover : Linear Ordered Crossover (LOX)
 // ----------------------------------------------------------------------------------
bool CLinearOrderedCrossover::operator()(CIndividual *child1, CIndividual *child2, const CIndividual &parent1, const CIndividual &parent2, double cr) const
{
	//const size_t Depot = parent1.vars()[0]; // 現在沒depot在x
	//std::cout << "in LOX" << endl;
	if (MathAux::random(0.0, 1.0) > cr) return false; // not crossovered

	child1->speed().resize(parent1.speed().size());
	child2->speed().resize(parent2.speed().size());
	CIndividual::TDecVec *children[] = { &child1->vars(), &child2->vars() };
	CIndividual::TObjVec *c_speeds[] = { &child1->speed(), &child2->speed() };
	const CIndividual::TDecVec *parents[] = { &parent1.vars(), &parent2.vars() };

	for (size_t c = 0; c < 2; c += 1)
	{
		children[c]->resize(parents[c]->size());
		
		size_t cut1 = rand() % children[c]->size(), cut2 = rand() % children[c]->size();
		if (cut1 > cut2) swap(cut1, cut2);

		size_t father = c;

		set<CIndividual::TGene> exists;
		for (size_t i = cut1; i <= cut2; i += 1) // keep the enclosed section
		{
			(*children[c])[i] = (*parents[father])[i];
			(*c_speeds[c])[i] = parent1.speed()[i];
			exists.insert((*children[c])[i]);
		}
		//std::cout << "insert OK" << endl;
		size_t c_pos = 0, m_pos = c_pos; //Linear  c_pos -> 1 from head to tail 
		size_t mother = !c;
		//std::cout << "cut1 = " << cut1 << endl;
		while (c_pos < cut1) //前半
		{
			while (exists.count((*parents[mother])[m_pos]) > 0) m_pos = (m_pos + 1) % parents[mother]->size();

			(*children[c])[c_pos] = (*parents[mother])[m_pos];
			(*c_speeds[c])[c_pos] = parent2.speed()[m_pos];
			exists.insert((*children[c])[c_pos]);

			c_pos = c_pos + 1;
			//std::cout << "c_pos = " << c_pos << endl;
		}
		//std::cout << "front OK" << endl;
		c_pos = cut2 + 1;
		while (c_pos < children[c]->size()) //後半
		{
			while (exists.count((*parents[mother])[m_pos]) > 0) m_pos = (m_pos + 1) % parents[mother]->size();

			(*children[c])[c_pos] = (*parents[mother])[m_pos];
			(*c_speeds[c])[c_pos] = parent2.speed()[m_pos];
			exists.insert((*children[c])[c_pos]);

			c_pos = c_pos + 1;
		}
		//std::cout << "back OK" << endl;
		//swap((*children[c])[0], *find(children[c]->begin(), children[c]->end(), Depot));
	}// for - children 0 and 1
	/*for (size_t c = 0; c < 2; c += 1)
	{
		cout << "c" << c << "=";
		for (size_t i = 0; i < children[c]->size(); i++) {
			cout << (*children[c])[i] << ' ';
		}
		cout << endl;
	}
	system("pause");*/
	//std::cout << "finish LOX" << endl;
	return true;

}// COrderedCrossover::operator()