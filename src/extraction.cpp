#include <map>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "kcm.hpp"
#include "cim.hpp"
#include "extraction.hpp"
#include "kernel.hpp"
#include "literal.hpp"
#include "yyglobal.hpp"

using std::map;

int sumbv1 = 0;
int sumbv2 = 0;

bool fr_cube_intersection(const polynomial& P, monomial& m)
{
	int v = 1;
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

bool fr2_cube_intersection(const vector<polynomial>& vP, monomial& m)
{
	int v = 1;
	int bi = -1;
	int bj = -1;
	int bk = -1;
	int bl = -1;
	for (int i = 0; i < vP.size(); ++i)
	{
		for (int j = i + 1; j < vP.size(); ++j)
		{
			for (int k = 0; k < vP[i].size(); ++k)
			{
				for (int l = 0; l < vP[j].size(); ++l)
				{
					int mn = gcd_mn(vP[i][k], vP[j][l]);
					if (mn > v)
					{
						bi = i;
						bj = j;
						bk = k;
						bl = l;
						v = mn;
					}
				}
			}
		}
	}
	if (bi == -1)
	{
		return false;
	}
	m = gcd(vP[bi][bk], vP[bj][bl]);
	return true;
}
void substitution(vector<polynomial>& vP, const polynomial& s)
{
	if (s.size() != 1)
	{
		return;
	}
	if (s[0].multiplication_number() < 2)
	{
		return;
	}
	for (auto& P : vP)
	{
		int rs = 0;
		for (int i = 0; i < P.size(); ++i)
		{
			polynomial dres = P / s[0];
			if (dres.size() == 0)
			{
				continue;
			}
			rs += dres.size();
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
		if (rs != 0)
		{
			int64_t bv = rs * (s[0].multiplication_number() - 1);
			sumbv2 += bv;
			summul -= bv;
			std::cerr << "Step 2: Descrease by" << std::setw(7) << bv << "."
			          << "Total descreased:" << std::setw(8) << sumbv2 << "."
			          << "Remain" << std::setw(8) << summul
			          << "(" << std::setw(5) << double(summul) / osummul * 100 << "%)" << "\r";
		}
	}
}
void fr1_find_kernel_intersections(vector<polynomial>& vP)
{
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
		int li;
		li = literal_append_tmp();
		polynomial& P = vP[pi];
		polynomial bcok = P / bk;
		bcok.name() = literal_name(li);
		//rewrote P
		for (int i = 0; i < bcok.size(); ++i)
		{
			P.remove(bk * bcok[i]);
		}
		monomial nm(li);
		P += nm * bk;
		vP.push_back(bcok);
		int bv = bk.multiplication_number() * (bcok.size() - 1);
		sumbv1 += bv;
		summul -= bv;
		std::cerr << "Step 1: Descrease by" << std::setw(7) << bv << "."
		          << "Total descreased:" << std::setw(8) << sumbv1 << "."
		          << "Remain" << std::setw(8) << summul
		          << "(" << std::setw(5) << double(summul) / osummul * 100 << "%)" << "\r";
	}
}
void fr2_find_kernel_intersections(vector<polynomial>& vP)
{
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
		sumbv1 += bv;
		summul -= bv;
		std::cerr << "Step 1: Descrease by" << std::setw(7) << bv << "."
		          << "Total descreased:" << std::setw(8) << sumbv1 << "."
		          << "Remain" << std::setw(8) << summul
		          << "(" << std::setw(5) << double(summul) / osummul * 100 << "%)" << "\r";
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
void fr_find_kernel_intersections(vector<polynomial>& vP)
{
	if (frkernelmode == "recursive")
	{
		fr2_find_kernel_intersections(vP);
	}
	else if (frkernelmode == "iterative")
	{
		fr1_find_kernel_intersections(vP);
	}
	else
	{
		std::cerr << "ERROR: Unknown frkernelmode " << frkernelmode << std::endl;
		exit(1);
	}
}

void fr1_find_cube_intersections(vector<polynomial>& vP)
{
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
	}
}
void fr2_find_cube_intersections(vector<polynomial>& vP)
{
	while (true)
	{
		monomial m;
		if (!fr2_cube_intersection(vP, m))
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
	}
}
void find_kernel_intersections(vector<polynomial>& vP)
{
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
			sumbv1 += bv;
			summul -= bv;
			std::cerr << "Step 1: Descrease by" << std::setw(7) << bv << "."
			          << "Total descreased:" << std::setw(8) << sumbv1 << "."
			          << "Remain" << std::setw(8) << summul
			          << "(" << std::setw(5) << double(summul) / osummul * 100 << "%)" << "\r";
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
				if (!flag)
				{
					continue;
				}
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
void old_find_cube_intersections(vector<polynomial>& vP)
{
	while (true)
	{
		cim tm(vP);
		monomial m;
		if (!tm.generate_best_rectangle(m))
		{
			return;
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
	}
}
void find_cube_intersections(vector<polynomial>& vP)
{
	if (cubemode == "together")
	{
		fr1_find_cube_intersections(vP);
	}
	else if (cubemode == "different")
	{
		fr1_find_cube_intersections(vP);
	}
	else if (cubemode == "old")
	{
		old_find_cube_intersections(vP);
	}
	else if (cubemode == "ignore")
	{
		return;
	}
	else
	{
		std::cerr << "ERROR:" << "Unknown cubemode " << cubemode;
		exit(1);
	}
}
