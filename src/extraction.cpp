#include <map>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include "kcm.hpp"
#include "cim.hpp"
#include "extraction.hpp"
#include "kernel.hpp"
#include "literal.hpp"

using std::map;

void find_kernel_intersections(vector<polynomial>& vP)
{
	//first, resize
	for (auto& P : vP) P.resize(literal_size());

	int i  = 0;
	while (true)
	{
		++i;
		vector<pair<monomial, polynomial>> vmp;
		vector<vector<pair<monomial, polynomial>>> vkmap;
		for (auto P : vP)
		{
			find_kernels(P, vmp);
			vkmap.push_back(std::move(vmp));
		}
		kcm tm(vkmap, vP);
		vkmap.clear();
		vector<int> vr;
		vector<int> vc;
		if (!tm.generate_best_rectangle(vr, vc)) return;
		else
		{
			//build new literal polynomial
			polynomial nlp;
			int li;
			li = literal_append_tmp();
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
			vP.push_back(nlp);
			vP.back().name() = literal_name(li);
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
		nl.resize(li + 1);
		nl += m;
		nl.name() = literal_name(li);
		vP.push_back(nl);
	}
}
