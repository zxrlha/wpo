#include <map>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include "kcm.hpp"
#include "cim.hpp"
#include "extraction.hpp"
#include "kernel.hpp"
#include "literal.hpp"

using std::map;

void substitution(vector<polynomial>& vP, const polynomial& s)
{
	if (s.size() != 1) return;
	if (s[0].multiplication_number() < 2) return;
	for (auto& P: vP)
	{
		for (int i = 0; i < P.size(); ++i)
		{
			polynomial dres = P / s[0];
			int li = literal_get(s.name());
			if (li == -1)
			{
				li = literal_append(s.name());
			}
			monomial nl(li);
			polynomial nP(P);
			for (int j = 0; j < dres.size(); ++j)
			{
				nP.remove(dres[0]*s[0]);
				nP += dres[0] * nl;
			}
			nP.name() = P.name();
			P = nP;
		}
	}
}

void fr_find_kernel_intersections(vector<polynomial>& vP)
{
	int sumbv = 0;
	int pi = 0;
	while (true)
	{
		monomial bk;
		polynomial bcok;
		int bv = 0;
		for (; pi < vP.size(); ++pi)
		{
			fr_find_kernels(vP[pi], bk, bcok, bv);
			if (bv != 0)
			{
				break;
			}
		}
		if (bv == 0)
		{
			break;
		}
		sumbv += bv;
		//std::cerr<<sumbv<<" "<<bv<<std::endl;
		//build new literal polynomial
		int li;
		li = literal_append_tmp();
		bcok.name() = literal_name(li);
		polynomial& P = vP[pi];
		//rewrote P
		for (int i = 0; i < bcok.size(); ++i)
		{
			P.remove(bk * bcok[i]);
		}
		monomial nm(li);
		P += nm * bk;
		vP.push_back(bcok);
	}
}

void find_kernel_intersections(vector<polynomial>& vP)
{
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
				monomial nm(li);
				P += nm * tm.row(vri);
			}
			vP.push_back(nlp);
			vP.back().name() = literal_name(li);
		}
	}
}

void find_cube_intersections(vector<polynomial>& vP)
{
	while (true)
	{
		cim tm(vP);
		monomial m;
		if (!tm.generate_best_rectangle(m)) return;
		int li = literal_append_tmp();
		//rewrote vP
		for (auto& P : vP)
		{
			polynomial dres = P / m;
			if (dres.size() == 0) continue;
			assert(dres.size() == 1);
			polynomial nP(P);
			nP.remove(dres[0]*m);
			nP.name() = P.name();
			monomial nl(li);
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
