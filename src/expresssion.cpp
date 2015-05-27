#incldue "expression.hpp"
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
