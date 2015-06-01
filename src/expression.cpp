#include <functional>
#include <algorithm>
#include "expression.hpp"
#include "literal.hpp"

using std::min;
using std::max;

monomial& monomial::operator*=(const monomial& m2)
{
	if (m2.size() > size())
	{
		resize(m2.size());
	}
	for (int i = 0; i < m2.size(); ++i)
	{
		_vpow[i] += m2[i];
	}
	_posi = !(_posi ^ m2._posi);
	return *this;
}

void polynomial::resize(int ns)
{
	assert(ns >= _size);
	for (auto& m : _vmon)
	{
		m.resize(ns);
	}
	_size = ns;
}

polynomial& polynomial::operator+=(const monomial& A)
{
	if (A.size() < _size)
	{
		monomial B(A);
		B.resize(_size);
		_vmon.push_back(B);
	}
	else
	{
		if (A.size() > _size)
		{
			resize(A.size());
		}
		_vmon.push_back(A);
	}
}

polynomial& polynomial::operator+=(monomial&& A)
{
	if (A.size() < _size)
	{
		A.resize(_size);
	}
	else if (A.size() > _size)
	{
		resize(A.size());
	}
	_vmon.push_back(std::move(A));
}

polynomial& polynomial::operator-=(const monomial& A)
{
	if (A.size() < _size)
	{
		monomial B(A);
		B.resize(_size);
		_vmon.push_back(B);
	}
	else
	{
		if (A.size() > _size)
		{
			resize(A.size());
		}
		_vmon.push_back(A);
	}
	_vmon.back().reverse_sign();
}

polynomial& polynomial::operator-=(monomial&& A)
{
	A.reverse_sign();
	if (A.size() < _size)
	{
		A.resize(_size);
	}
	else if (A.size() > _size)
	{
		resize(A.size());
	}
	_vmon.push_back(std::move(A));
}
polynomial operator+(const monomial& A, const monomial& B)
{
	polynomial P(A);
	P += B;
	return P;
}

polynomial operator-(const monomial& A, const monomial& B)
{
	polynomial P(A);
	P -= B;
	return P;
}

polynomial& polynomial::operator*=(const monomial& A)
{
	for (auto& m : _vmon)
	{
		m *= A;
	}
	return *this;
}

polynomial operator*(const polynomial& P, const monomial& A)
{
	polynomial res(P);
	res *= A;
	return res;
}

polynomial operator+(const polynomial& P, const monomial& A)
{
	polynomial res(P);
	res += A;
	return res;
}

polynomial operator-(const polynomial& P, const monomial& A)
{
	polynomial res(P);
	res -= A;
	return res;
}

monomial operator*(const monomial& A, const monomial& B)
{
	monomial C(A);
	C *= B;
	return C;
}

polynomial operator+(const polynomial& P1, const polynomial& P2)
{
	polynomial res(P1);
	for (int i = 0; i < P2.number(); ++i)
	{
		res += P2[i];
	}
	return res;
}

polynomial operator-(const polynomial& P1, const polynomial& P2)
{
	polynomial res(P1);
	for (int i = 0; i < P2.number(); ++i)
	{
		res -= P2[i];
	}
	return res;
}

monomial polynomial::gcd() const
{
	assert(_vmon.size() != 0);
	monomial A;
	A.resize(_size);
	for (int i = 0; i < _size; ++i)
	{
		A[i] = _vmon[0][i];
		for (int j = 1; j < _vmon.size(); ++j)
		{
			if (_vmon[j][i] < A[i]) A[i] = _vmon[j][i];
		}
	}
	return A;
}

polynomial operator/(const polynomial& P, const monomial& A)
{
	assert(P.size() >= A.size());
	polynomial res;
	for (int i = 0; i < P.number(); ++i)
	{
		bool flag = true;
		for (int j = 0; j < A.size(); ++j)
		{
			if (P[i][j] < A[j])
			{
				flag = false;
				break;
			}
		}
		if (flag)
		{
			monomial C = P[i];
			for (int j = 0; j < A.size(); ++j)
			{
				C[j] -= A[j];
			}
			res += C;
		}
	}
	return res;
}

polynomial mod(const polynomial& P, const monomial& A)
{
	assert(P.size() >= A.size());
	polynomial res;
	for (int i = 0; i < P.number(); ++i)
	{
		bool flag = true;
		for (int j = 0; j < A.size(); ++j)
		{
			if (P[i][j] < A[j])
			{
				flag = false;
				break;
			}
		}
		if (!flag)
		{
			res += P[i];
		}
	}
	return res;
}

bool operator<(const monomial& A, const monomial& B)
{
	for (int i = 0; i < min(A.size(), B.size()); ++i)
	{
		if (A[i] < B[i]) return true;
		if (A[i] > B[i]) return false;
	}
	if (A.size() == B.size())//now A=B except sign
	{
		if (A.is_negative() && B.is_positive()) return true;
		else return false;
	}
	if (A.size() > B.size())
	{
		for (int i = B.size(); i < A.size(); ++i)
		{
			if (A[i] != 0)
				return false;
		}
		//now A=B except sign
		if (A.is_negative() && B.is_positive()) return true;
		else return false;
	}
	//now B.size() > A.size()
	for (int i = A.size(); i < B.size(); ++i)
	{
		if (B[i] != 0)
			return true;
	}
	//now A=B except sign
	if (A.is_negative() && B.is_positive()) return true;
	else return false;
}

bool operator==(const monomial& A, const monomial& B)
{
	for (int i = 0; i < min(A.size(), B.size()); ++i)
	{
		if (A[i] != B[i]) return false;
	}
	if (A.is_positive() != B.is_positive()) return false;
	if (A.size() > B.size())
	{
		for (int i = B.size(); i < A.size(); ++i)
		{
			if (A[i] != 0)
				return false;
		}
		return true;
	}
	//now B.size() > A.size()
	for (int i = A.size(); i < B.size(); ++i)
	{
		if (B[i] != 0)
			return false;
	}
	return true;
}

/*
bool operator<(const polynomial& A, const polynomial& B)
{
    if (A.number() > B.number()) return false;
    if (A.number() < B.number()) return true;
    for (int i = 0; i < A.number(); ++i)
    {
        if (A[i] > B[i]) return false;
        if (A[i] < B[i]) return true;
    }
    return false;
}

bool operator==(const polynomial& A, const polynomial& B)
{
    if (A.number() != B.number()) return false;
    for (int i = 0; i < A.number(); ++i)
    {
        if (A[i] != B[i]) return false;
    }
    return true;
}
*/

bool polynomial::contain(const monomial& m) const
{
	return std::find(_vmon.begin(), _vmon.end(), m) != _vmon.end();
}

int monomial::multiplication_number() const
{
	int res = 0;
	for (int i = 0; i < _vpow.size(); ++i)
	{
		res += _vpow[i];
	}
	return res;
}

ostream& operator<<(ostream& os, const monomial& m)
{
	if (m.is_negative()) os << "-";
	int status = 0;
	for (int i = 0; i < m.size(); ++i)
	{
		if (m[i] != 0)
		{
			for (int j = 0; j < m[i]; ++j)
			{
				if (status != 0) os << "*";
				os << literal_name(i);
				status = 1;
			}
		}
	}
	if (status == 0) os << "1";
	return os;
}

ostream& operator<<(ostream& os, const polynomial& p)
{
	os << p.name() << "=";
	for (int i = 0; i < p.number(); ++i)
	{
		if (i != 0 && p[i].is_positive()) os << "+";
		os << p[i];
	}
	return os;
}

void polynomial::remove(const monomial& m)
{
	auto it = std::find(_vmon.begin(), _vmon.end(), m);
	assert(*it == m);
	_vmon.erase(it);
}

bool polynomial::contain_literals(const set<int>& ls)
{
	for (int i = 0; i < _vmon.size(); ++i)
	{
		for (auto it : ls)
		{
			if (_vmon[i][it] != 0) return true;
		}
	}
	return false;
}

set<int> polynomial::literals() const
{
	set<int> res;
	for (int i = 0; i < _vmon.size(); ++i)
	{
		for (int j = 0; j < _vmon[i].size(); ++j)
		{
			if (_vmon[i][j] != 0) res.insert(j);
		}
	}
	return res;
}

set<int> polynomial::tmp_literals() const
{
	set<int> res;
	for (int i = 0; i < _vmon.size(); ++i)
	{
		for (int j = 0; j < _vmon[i].size(); ++j)
		{
			if (_vmon[i][j] != 0 && literal_is_tmp(j)) res.insert(j);
		}
	}
	return res;
}
