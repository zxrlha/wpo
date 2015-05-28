#ifndef WPO_EXPRESSION_HPP
#define WPO_EXPRESSION_HPP 1

#include <vector>
#include <cassert>
#include <string>

using std::vector;
using std::string;

class monomial
{
public:
	monomial()
	{
		_posi = true;
	};
	bool is_positive() const
	{
		return _posi;
	}
	bool is_negative() const
	{
		return !_posi;
	}

	void reverse_sign()
	{
		_posi = !_posi;
	}

	int size() const
	{
		return _vpow.size();
	}
	void resize(int newsize)
	{
		assert(newsize >= size());
		_vpow.resize(newsize);
	}

	int operator[](int i) const
	{
		return _vpow[i];
	}

	int& operator[](int i)
	{
		return _vpow[i];
	}
	monomial& operator*=(const monomial& m2);

	int multiplication_number() const;
protected:
	vector<int> _vpow;
	bool _posi;
};

class polynomial
{
public:
	polynomial()
	{
		_size = 0;
	}
	explicit polynomial(const monomial& A)
		: _vmon{A}
	{
		_size = A.size();
	};
	int size() const
	{
		return _size;
	}
	void resize(int ns);
	int number() const
	{
		return _vmon.size();
	}
	polynomial& operator+=(const monomial& A);
	polynomial& operator-=(const monomial& A);

	const monomial& operator[](int i) const
	{
		return _vmon[i];
	}

	monomial gcd() const;
	bool contain(const monomial& m) const;
	string& name() { return _name; };
protected:
	void sort();
	vector<monomial> _vmon;
	int _size;
	string _name;
};

polynomial operator+(const monomial& A, const monomial& B);
polynomial operator-(const monomial& A, const monomial& B);
monomial operator*(const monomial& A, const monomial& B);
polynomial operator/(const polynomial& P, const monomial& A);

polynomial mod(const polynomial& P, const monomial& A);

bool operator<(const monomial& A, const monomial& B);
inline bool operator>(const monomial& A, const monomial& B)
{
	return B < A;
}
inline bool operator<=(const monomial& A, const monomial& B)
{
	return !(A > B);
}
inline bool operator>=(const monomial& A, const monomial& B)
{
	return !(A < B);
}
bool operator==(const monomial& A, const monomial& B);
inline bool operator!=(const monomial& A, const monomial& B)
{
	return !(A == B);
}

bool operator<(const polynomial& A, const polynomial& B);
inline bool operator>(const polynomial& A, const polynomial& B)
{
	return B < A;
}
inline bool operator<=(const polynomial& A, const polynomial& B)
{
	return !(A > B);
}
inline bool operator>=(const polynomial& A, const polynomial& B)
{
	return !(A < B);
}
bool operator==(const polynomial& A, const polynomial& B);
inline bool operator!=(const polynomial& A, const polynomial& B)
{
	return !(A == B);
}

#endif
