#ifndef WPO_EXPRESSION_HPP
#define WPO_EXPRESSION_HPP 1

#include <vector>
#include <set>
#include <cassert>
#include <string>
#include <iostream>

using std::set;
using std::vector;
using std::string;
using std::ostream;

class funcexpr
{
public:
	string _funcname;
	int _paraid;
	string _resname;
};

class monomial
{
public:
	//constructor
	monomial()
	{
		_coef = 1;
	};

	explicit monomial(int l, int p = 1)
		:_vterm{std::make_pair(l, p)}
	{
		_coef = 1;
	}
	monomial(const monomial&) = default;
	monomial(monomial&&) = default;

	//append, the caller should ensure correct order
	void append(int l, int p = 1)
	{
		_vterm.push_back(std::make_pair(l, p));
	}

	//assignment
	monomial& operator=(const monomial&) = default;
	monomial& operator=(monomial&&) = default;

	//sign information
	bool is_positive() const
	{
		return _coef > 0;
	}
	bool is_negative() const
	{
		return !is_positive();
	}

	//coefficient information
	int64_t coef() const
	{
		return _coef;
	}
	int64_t& coef()
	{
		return _coef;
	}

	//sign operation
	void reverse_sign()
	{
		_coef = -_coef;
	}

	//term access
	int getpow(int i) const;
	//void setpow(int i, int j);

	//non-zero term access
	int lit(int i) const
	{
		return _vterm[i].first;
	}
	int pow(int i) const
	{
		return _vterm[i].second;
	}
	int size() const
	{
		return _vterm.size();
	}

	//ring operation
	monomial& operator*=(const monomial& m2);

	//divide operation, m2 | (*this) is assumed
	monomial& operator/=(const monomial& m2);

	//information for kcm
	int multiplication_number() const;
protected:
	vector<std::pair<int, int>> _vterm;
	int64_t _coef;
};

class polynomial
{
public:
	//constructor
	polynomial()
	{
	}
	explicit polynomial(const monomial& A)
		: _vmon{A}
	{
	};
	polynomial(const polynomial&) = default;
	polynomial(polynomial&&) = default;

	//assignment
	polynomial& operator=(const polynomial&) = default;
	polynomial& operator=(polynomial&&) = default;

	int size() const
	{
		return _vmon.size();
	}

	//ring operation
	polynomial& operator+=(const monomial& A);
	polynomial& operator+=(monomial&& A);
	polynomial& operator-=(const monomial& A);
	polynomial& operator-=(monomial&& A);
	polynomial& operator*=(const monomial& A);

	//term access
	const monomial& operator[](int i) const
	{
		return _vmon[i];
	}

	//term access
	monomial& operator[](int i)
	{
		return _vmon[i];
	}

	monomial gcd() const;
	bool contain(const monomial& m) const;
	void remove(const monomial& m);

	//name information operation for output
	string& name()
	{
		return _name;
	};
	const string& name() const
	{
		return _name;
	};

	//literal information for output
	bool contain_literals(const set<int>& ls);
	set<int> literals() const;
	set<int> tmp_literals() const;

	//variable reduction
	int single_id() const;

	//multiplication number
	int multiplication_number() const;
protected:
	void sort();
	vector<monomial> _vmon;
	string _name;
};

//ring operation
polynomial operator+(const monomial& A, const monomial& B);
polynomial operator-(const monomial& A, const monomial& B);
monomial operator*(const monomial& A, const monomial& B);

//operation for kcm
polynomial operator/(const polynomial& P, const monomial& A);
//polynomial mod(const polynomial& P, const monomial& A);

//ring operator
polynomial operator+(const polynomial& P1, const polynomial& P2);
polynomial operator-(const polynomial& P1, const polynomial& P2);

polynomial operator+(const polynomial& P, const monomial& A);
polynomial operator-(const polynomial& P, const monomial& A);
polynomial operator*(const polynomial& P, const monomial& A);

inline polynomial operator+(const monomial& A, const polynomial& P)
{
	return P + A;
}
inline polynomial operator*(const monomial& A, const polynomial& P)
{
	return P * A;
}

//compare operator for sort and associated container
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

bool operator==(const polynomial& A, const polynomial& B);
/*
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
*/

//output operator for view and debug
ostream& operator<<(ostream& os, const monomial& m);
ostream& operator<<(ostream& os, const polynomial& m);

monomial gcd(const monomial& A, const monomial& B);
int gcd_mn(const monomial& A, const monomial& B);
#endif
