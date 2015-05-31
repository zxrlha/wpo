#include <map>
#include <boost/lexical_cast.hpp>
#include "kcm.hpp"
#include "cim.hpp"
#include "extraction.hpp"
#include "kernel.hpp"
#include "literal.hpp"

using std::map;

void find_kernel_intersections(vector<polynomial>& vP)
{
	while (true)
	{
		map<monomial, polynomial> vmp;
		vector<map<monomial, polynomial>> vkmap;
		for (auto P: vP)
		{
			find_kernels(P, vmp);
			vkmap.push_back(vmp);
		}
		kcm tm(vkmap, vP);
		vector<int> vr;
		vector<int> vc;
		map<polynomial, int> mR;
		if (!tm.generate_best_rectangle(vr, vc)) return;
		else
		{
			//build new literal polynomial
			polynomial nlp;
			for (auto vci : vc)
			{
				nlp += tm.column(vci);
			}
			for (auto vri : vr) //loop over co-kernel
			{
				int pi = tm.index(vri);//P index
				polynomial& P = vP[pi];
				bool flag = true;
				//check
				for (auto vci : vc)
				{
					if (!P.contain(tm.row(vri)*tm.column(vci)))
					{
						flag = false;
						break;
					}
				}
				if (!flag) continue;
				//append
				auto it = mR.find(nlp);
				int li;
				if (it != mR.end())
				{
					li = it->second;
				}
				else
				{
					li = literal_append_tmp();
					mR.insert(std::make_pair(nlp, li));
				}
				//rewrote P
				polynomial nP;
				for (auto vci : vc)
				{
					P.remove(tm.row(vri)*tm.column(vci));
				}
				monomial nm;
				nm.resize(li + 1);
				nm[li] = 1;
				P += nm * tm.row(vri);
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

void find_cube_intersections(vector<polynomial>& vP)
{
	while (true)
	{
		cim tm(vP);
		vector<int> vr;
		monomial m;
		if (!tm.generate_best_rectangle(vr, m)) return;
		int li = literal_append_tmp();
		//rewrote vP
		for (auto& P : vP)
		{
			P.resize(P.size() + 1);
			polynomial dres = P / m;
			if (dres.number() == 0) continue;
			assert(dres.number() == 1);
			polynomial nP = mod(P, m);
			nP.name() = P.name();
			monomial nl;
			nl.resize(li + 1);
			nl[li] = 1;
			nP += dres[0] * nl;
			P = nP;
		}
		//add this into vP
		polynomial nl;
		nl += m;
		nl.name() = literal_name(li);
		vP.push_back(nl);
	}
}
