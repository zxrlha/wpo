#include <algorithm>
#include "cim.hpp"

cim::cim(const vector<polynomial>& vP)
{
	for (auto P : vP)
	{
		for (int i = 0; i < P.number(); ++i)
		{
			_mat.push_back(P[i]);
		}
	}
}

bool cim::generate_best_rectangle(vector<int>& row, monomial& m)
{
	int max = 0;
	for (int i = 0; i < _mat.size(); ++i)
	{
		vector<int> vr{i};
		monomial tmpm(_mat[i]);
		if (tmpm.multiplication_number() <= 1) continue;
		int v = value_of_best_rectangle(i, vr, tmpm);
		if (v > max)
		{
			max = v;
			row = vr;
			m = tmpm;
		}
	}
	if (max == 0) return false;
	return true;
}

//m is gcd of included terms
int cim::value_of_best_rectangle(int ri, vector<int>& row, monomial& m)
{
	vector<int> bestvr(row);
	monomial bestm(m);
	int max = value_of_prime_rectangle(row, m);
	for (int i = 0; i < ri; ++i)
	{
		polynomial np = _mat[i] + m;
		monomial nm = np.gcd();
		if (nm.multiplication_number() <= 1) continue;
		vector<int> tr(row);
		tr.push_back(i);
		int tm = value_of_best_rectangle(i, tr, nm);
		if (tm > max)
		{
			max = tm;
			bestvr = tr;
			bestm = nm;
		}
	}
	row = bestvr;
	m = bestm;
	return max;
}

int cim::value_of_prime_rectangle(vector<int>& row, monomial& m)
{
	int R = row.size();
	return (R - 1) * (m.multiplication_number() - 1);
}
