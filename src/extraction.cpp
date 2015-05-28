#include <map>
#include <boost/lexical_cast.hpp>
#include "kcm.hpp"
#include "extraction.hpp"
#include "kernel.hpp"
#include "literal.hpp"

using std::map;

void find_kernel_intersections(vector<polynomial>& vP)
{
	while (true)
	{
		set<monomial> scok;
		set<polynomial> sk;
		find_kernels(vP, scok, sk);
		kcm tm(scok, sk, vP);
		vector<int> vr;
		vector<int> vc;
		map<polynomial, int> mR;//new intersections
		if (!tm.generate_best_rectangle(vr, vc)) return;
		else
		{
			for (auto vri: vr)//loop over co-kernel
			{
				for (auto& P : vP) //loop over expressions
				{
					polynomial dres = P / tm.row(vri);
					if (dres.number() >= 2 && dres.gcd() == monomial())//it is a co-kernel of P
					{
						//append
						auto it = mR.find(dres);
						int li;
						if (it != mR.end())
						{
							li = it->second;
						}
						else
						{
							li = literal_append_tmp();
							mR.insert(std::make_pair(dres, li));
						}
						//rewrote P
						polynomial nP = mod(P, tm.row(vri));
						monomial nm;
						nm.resize(li + 1);
						nm[li] = 1;
						nP += nm * tm.row(vri);
						nP.name() = P.name();
						P = nP;
					}
				}
			}
		}
		for (auto p : mR)
		{
			vP.push_back(p.first);
			vP.back().name() = literal_name(p.second);
		}
		for (auto& p : vP)
		{
			p.resize(literal_size());
		}
	}
}
