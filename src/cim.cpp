#include <algorithm>
#include <list>
#include "cim.hpp"
#include "literal.hpp"

int gcd_mn(const monomial& A, const monomial& B)
{
	int sum = 0;
	int i = 0;
	int j = 0;
	while (i != A.size() && j != B.size())
	{
		if (A.lit(i) < B.lit(j))
		{
			++i;
		}
		else if (A.lit(i) > B.lit(j))
		{
			++j;
		}
		else
		{
			sum += std::min(A.pow(i), B.pow(j));
			++i;
			++j;
		}
	}
	return sum;
}

monomial gcd(const monomial& A, const monomial& B)
{
	monomial res;
	int i = 0;
	int j = 0;
	while (i != A.size() && j != B.size())
	{
		if (A.lit(i) < B.lit(j))
		{
			++i;
		}
		else if (A.lit(i) > B.lit(j))
		{
			++j;
		}
		else
		{
			res.append(A.lit(i), std::min(A.pow(i), B.pow(j)));
			++i;
			++j;
		}
	}
	return res;
}

cim::cim(const vector<polynomial>& vP)
{
	for (auto P : vP)
	{
		for (int i = 0; i < P.size(); ++i)
		{
			if (P[i].multiplication_number() > 1)
			{
				_mat.push_back(P[i]);
			}
		}
	}
	//std::cout << _mat.size() << std::endl;
}

bool cim::generate_best_rectangle(monomial& m)
{
	_bv = 0;
	_bs = 0;
	_bm = monomial();
	for (int i = _mat.size()-1; i >= 0; --i)
	{
		generate_best_rectangle(i);
	}
	if (_bv == 0)
	{
		return false;
	}
	m = _bm;
	return true;
}

void cim::generate_best_rectangle(int ri)
{
	//std::cerr << "RI:" << ri << std::endl;
	vector<int> posi;
	int rowsize = 1;
	monomial m(_mat[ri]);
	m.coef() = 1;
	int mmn = m.multiplication_number();
	for (int i = 0; i < ri; ++i)
	{
		int mn = gcd_mn(_mat[i], m);
		if (mn <= 1)
		{
			continue;
		}
		else if (mn == mmn)
		{
			++rowsize;
		}
		else
		{
			posi.push_back(i);
		}
	}
	generate_best_rectangle(rowsize, m, posi);
}

void cim::generate_best_rectangle(int rowsize, const monomial& m, vector<int>& posi)
{
	int tmn = m.multiplication_number();
	int v = value_of_prime_rectangle(rowsize, tmn);
	if (v > _bv)
	{
		_bv = v;
		_bs = rowsize;
		_bm = m;
	}
	v = value_of_prime_rectangle(rowsize + posi.size(), tmn - 1);
	if (v < _bv)
	{
		return;
	}
	for (auto i: posi)
	{
		monomial nm = gcd(_mat[i], m);//require results
		int nrs = rowsize + 1;
		int mmn = nm.multiplication_number();
		if (value_of_prime_rectangle(rowsize+posi.size(), mmn) < _bv) continue;
		vector<int> nposi;
		for (auto ni : posi)
		{
			if (ni <= i) continue;
			int mn = gcd_mn(_mat[ni], nm);
			if (mn <= 1)
			{
				continue;
			}
			else if (mn == mmn)
			{
				++nrs;
			}
			else
			{
				nposi.push_back(ni);
			}
		}
		generate_best_rectangle(nrs, nm, nposi);
	}
}

int cim::value_of_prime_rectangle(int rowsize, int mn)
{
	return (rowsize - 1) * (mn - 1);
}
