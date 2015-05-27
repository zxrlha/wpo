#include "expression.hpp"
#include <functional>
#include <algorithm>

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
	for (auto m : _vmon)
	{
		m.resize(ns);
	}
	_size = ns;
}

void polynomial::sort()
{
	std::sort(_vmon.begin(), _vmon.end());
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
	this->sort();
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
	this->sort();
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

monomial operator*(const monomial& A, const monomial& B)
{
	monomial C(A);
	C *= B;
	return C;
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

