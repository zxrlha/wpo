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

class monomial
{
public:
	//constructor
	monomial()
	{
		_posi = true;
	};
	monomial(const monomial&) = default;
	monomial(monomial&&) = default;

	//assignment
	monomial& operator=(const monomial&) = default;
	monomial& operator=(monomial&&) = default;

	//sign information
	bool is_positive() const
	{
		return _posi;
	}
	bool is_negative() const
	{
		return !_posi;
	}

	//sign operation
	void reverse_sign()
	{
		_posi = !_posi;
	}

	//size information and operation
	int size() const
	{
		return _vpow.size();
	}
	void resize(int newsize)
	{
		assert(newsize >= size());
		_vpow.resize(newsize);
	}

	//term access
	int operator[](int i) const
	{
		return _vpow[i];
	}
	int& operator[](int i)
	{
		return _vpow[i];
	}

	//ring operation
	monomial& operator*=(const monomial& m2);

	//information for kcm
	int multiplication_number() const;
protected:
	vector<int> _vpow;
	bool _posi;
};

class polynomial
{
public:
	//constructor
	polynomial()
	{
		_size = 0;
	}
	explicit polynomial(const monomial& A)
		: _vmon{A}
	{
		_size = A.size();
	};
	polynomial(const polynomial&) = default;
	polynomial(polynomial&&) = default;

	//assignment
	polynomial& operator=(const polynomial&) = default;
	polynomial& operator=(polynomial&&) = default;

	//size information and operation
	int size() const
	{
		return _size;
	}
	void resize(int ns);
	int number() const
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

	//information and operation for kcm
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
protected:
	vector<monomial> _vmon;
	int _size;
	string _name;
};

//ring operation
polynomial operator+(const monomial& A, const monomial& B);
polynomial operator-(const monomial& A, const monomial& B);
monomial operator*(const monomial& A, const monomial& B);

//operation for kcm
polynomial operator/(const polynomial& P, const monomial& A);
polynomial mod(const polynomial& P, const monomial& A);

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
bool nosort_equal(const polynomial& A, const polynomial& B);

//output operator for view and debug
ostream& operator<<(ostream& os, const monomial& m);
ostream& operator<<(ostream& os, const polynomial& m);
#endif
