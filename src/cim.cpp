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
	_br.clear();
	_bm = monomial();
	_bv = 0;
	for (int i = 0; i < _mat.size(); ++i)
	{
		vector<int> vr{i};
		monomial tmpm(_mat[i]);
		if (tmpm.multiplication_number() <= 1) continue;
		generate_best_rectangle(i, vr, tmpm);
	}
	if (_bv == 0) return false;
	row = _br;
	m = _bm;
	return true;
}

//m is gcd of included terms
void cim::generate_best_rectangle(int ri, vector<int>& row, const monomial& m)
{
	int v = value_of_prime_rectangle(row, m);
	if (v > _bv)
	{
		_bv = v;
		_br = row;
		_bm = m;
	}
	for (int i = 0; i < ri; ++i)
	{
		polynomial np = _mat[i] + m;
		monomial nm = np.gcd();
		if (nm.multiplication_number() <= 1) continue;
		row.push_back(i);
		generate_best_rectangle(i, row, nm);
		row.erase(row.end()-1);
	}
}

int cim::value_of_prime_rectangle(vector<int>& row, const monomial& m)
{
	int R = row.size();
	return (R - 1) * (m.multiplication_number() - 1);
}
