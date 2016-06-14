#include <map>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "kcm.hpp"
#include "extraction.hpp"
#include "kernel.hpp"
#include "literal.hpp"
#include "yyglobal.hpp"

using std::map;

void kcm_find_kernel_intersections(vector<polynomial>& vP)
{
	int sumbv1 = 0;
	int bv = 0;
	while (true)
	{
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
		bv = tm.generate_best_rectangle(vr, vc);
		if (bv < 0)
		{
			break;
		}
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
			bv = 0;
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
				if (!flag)
				{
					continue;
				}
				//rewrote P
				polynomial nP;
				int64_t before = P.multiplication_number();
				for (auto vci : vc)
				{
					P.remove(tm.row(vri)*tm.column(vci));
				}
				monomial nm(li);
				P += nm * tm.row(vri);
				int64_t after = P.multiplication_number();
				bv += before - after;
			}
			vP.push_back(nlp);
			vP.back().name() = literal_name(li);
			literal_set_ring_level(li, nlp.ring_level());
			bv -= nlp.multiplication_number();
			sumbv1 += bv;
			summul -= bv;
			std::cerr << "Step 1: Descrease by" << std::setw(7) << bv << "."
			          << "Total descreased:" << std::setw(8) << sumbv1 << "."
			          << "Remain" << std::setw(8) << summul
			          << "(" << std::setw(5) << double(summul) / osummul * 100 << "%)" << "\r";
		}
	}
}

bool fr_cube_intersection(const polynomial& P, monomial& m)
{
	int v = 0;
	int bi = -1;
	int bj = -1;
	for (int i = 0; i < P.size(); ++i)
	{
		for (int j = i + 1; j < P.size(); ++j)
		{
			int mn = gcd_mn(P[i], P[j]);
			if (mn > v)
			{
				bi = i;
				bj = j;
				v = mn;
			}
		}
	}
	if (bi == -1)
	{
		return false;
	}
	m = gcd(P[bi], P[bj]);
	return true;
}

void fr_find_kernel_intersections(vector<polynomial>& vP)
{
	int sumbv1 = 0;
	int pi = 0;
	while (true)
	{
		monomial bk;
		bool flag = flag;
		for (; pi < vP.size(); ++pi)
		{
			flag = fr_cube_intersection(vP[pi], bk);
			if (flag)
			{
				break;
			}
		}
		if (!flag)
		{
			break;
		}
		//build new literal polynomial
		polynomial& P = vP[pi];
		polynomial bcok = P / bk;
		int64_t before = P.multiplication_number();
		//rewrote P
		for (int i = 0; i < bcok.size(); ++i)
		{
			P.remove(bk * bcok[i]);
		}
		int li;
		li = vP_get(bcok);
		bool newflag = false;
		if (li == -1)
		{
			newflag = true;
			li = literal_append_tmp();
		}
		else
		{
			li = literal_get(vP[li].name());
		}
		bcok.name() = literal_name(li);
		monomial nm(li);
		P += nm * bk;
		int64_t after = P.multiplication_number();
		if (newflag)
		{
			vP.push_back(bcok);
			literal_set_ring_level(li, bcok.ring_level());
		}
		if (newflag)
		{
			after += bcok.multiplication_number();
		}
		int bv = before - after;
		sumbv1 += bv;
		summul -= bv;
		std::cerr << "Step 1: Descrease by" << std::setw(7) << bv << "."
		          << "Total descreased:" << std::setw(8) << sumbv1 << "."
		          << "Remain" << std::setw(8) << summul
		          << "(" << std::setw(5) << double(summul) / osummul * 100 << "%)" << "\r";
	}
}

void find_kernel_intersections(vector<polynomial>& vP)
{
	if (strategy == "kcm")
	{
		kcm_find_kernel_intersections(vP);
	}
	else if (strategy == "fastrun")
	{
		fr_find_kernel_intersections(vP);
	}
	else
	{
		std::cerr << "ERROR: unknown strategy " << strategy << std::endl;
		error_end();
	}
	std::cerr << std::endl;
}

bool fr_cube_intersection(const vector<polynomial>& vP, monomial& m)
{
	std::vector<monomial> mat;
	for (int i = 0; i < vP.size(); ++i)
	{
		for (int j = 0; j < vP[i].size(); ++j)
		{
			mat.push_back(vP[i][j]);
		}
	}
	int v = 1;
	int bi = -1;
	int bj = -1;
	for (int i = 0; i < mat.size(); ++i)
	{
		for (int j = i + 1; j < mat.size(); ++j)
		{
			int mn = gcd_mn(mat[i], mat[j]);
			if (mn > v)
			{
				bi = i;
				bj = j;
				v = mn;
			}
		}
	}
	if (bi == -1)
	{
		return false;
	}
	m = gcd(mat[bi], mat[bj]);
	return true;
}

void find_cube_intersections(vector<polynomial>& vP)
{
	int sumbv2 = 0;
	while (true)
	{
		monomial m;
		if (!fr_cube_intersection(vP, m))
		{
			break;
		}
		int li = literal_append_tmp();
		//rewrote vP
		int rs = 0;
		for (auto& P : vP)
		{
			polynomial dres = P / m;
			if (dres.size() == 0)
			{
				continue;
			}
			++rs;
			polynomial nP(P);
			for (int i = 0; i < dres.size(); ++i)
			{
				nP.remove(dres[i]*m);
			}
			nP.name() = P.name();
			monomial nl(li);
			for (int i = 0; i < dres.size(); ++i)
			{
				nP += dres[i] * nl;
			}
			P = nP;
		}
		int64_t bv = (rs - 1) * (m.multiplication_number() - 1);
		sumbv2 += bv;
		summul -= bv;
		std::cerr << "Step 2: Descrease by" << std::setw(7) << bv << "."
		          << "Total descreased:" << std::setw(8) << sumbv2 << "."
		          << "Remain" << std::setw(8) << summul
		          << "(" << std::setw(5) << double(summul) / osummul * 100 << "%)" << "\r";
		//add this into vP
		polynomial nl;
		nl += m;
		nl.name() = literal_name(li);
		vP.push_back(nl);
		literal_set_ring_level(li, nl.ring_level());
	}
	std::cerr << std::endl;
}
