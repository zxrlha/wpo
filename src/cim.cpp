#include <algorithm>
#include <list>
#include "cim.hpp"
#include "literal.hpp"

llmonomial gcd(const monomial& A, const llmonomial& B, bool& flag)
{
	llmonomial R(B);
	flag = true;
	for (auto it = R._vpow.begin(); it != R._vpow.end();)
	{
		int a = A[it->first];
		if (a == 0)
		{
			it = R._vpow.erase(it);
			flag = false;
		}
		else
		{
			if (a < it->second)
			{
				it->second = a;
				flag = false;
			}
			++it;
		}
	}
	//std::cout<<B._vpow.size()<<std::endl;
	return R;
}

monomial tomonomial(const llmonomial& L)
{
	monomial m;
	m.resize(literal_size());
	for (auto it : L._vpow)
	{
		m[it.first] = it.second;
	}
	return m;
}

cim::cim(const vector<polynomial>& vP)
{
	for (auto P : vP)
	{
		for (int i = 0; i < P.number(); ++i)
		{
			if (P[i].multiplication_number() > 1)
				_mat.push_back(P[i]);
		}
	}
	//std::cout << _mat.size() << std::endl;
}

bool cim::generate_best_rectangle(vector<int>& row, monomial& m)
{
	_bv = 0;
	_br.clear();
	_bm = monomial();
	for (int i = 0; i < _mat.size(); ++i)
	{
		generate_best_rectangle(i);
		//std::cout << i << std::endl;
	}
	if (_bv == 0) return false;
	row = _br;
	m = _bm;
	return true;
}

void cim::generate_best_rectangle(int ri)
{
	set<int> posi;
	vector<int> row{ri};
	llmonomial m(_mat[ri]);
	for (int i = 0; i < ri; ++i)
	{
		bool flag;
		llmonomial nm = gcd(_mat[i], m, flag);
		if (nm.multiplication_number() <= 1) continue;
		if (flag)
		{
			row.push_back(i);
		}
		else
		{
			posi.insert(i);
		}
	}
	generate_best_rectangle(row, m, posi);
}

void cim::generate_best_rectangle(vector<int>& row, const llmonomial& m, set<int> posi)
{
	int v = value_of_prime_rectangle(row, m);
	if (v > _bv)
	{
		_bv = v;
		_br = row;
		_bm = tomonomial(m);
	}
	int ls = row.size();
	//pick one
	for (auto it = posi.begin(); it != posi.end();)
	{
		bool tf;
		llmonomial nm = gcd(_mat[*it], m, tf);
		set<int> nposi;
		for (auto ni:posi)
		{
			bool flag;
			llmonomial tnm = gcd(_mat[ni], nm, flag);
			if (tnm.multiplication_number() <= 1) continue;
			if (flag)
			{
				row.push_back(ni);
			}
			else
			{
				nposi.insert(ni);
			}
		}
		generate_best_rectangle(row, nm, nposi);
		row.resize(ls);
		it = posi.erase(it);
	}
}

int cim::value_of_prime_rectangle(vector<int>& row, const llmonomial& m)
{
	int R = row.size();
	return (R - 1) * (m.multiplication_number() - 1);
}
