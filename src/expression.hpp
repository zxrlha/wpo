#ifndef WPO_EXPRESSION_HPP
#define WPO_EXPRESSION_HPP 1

#include <vector>
#include <cassert>

using std::vector;

class monomial
{
public:
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

	int operator[](size_t i) const
	{
		return _vpow[i];
	}

	monomial& operator*=(const monomial& m2);
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
		_mono_size = A.size();
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

	const monomial& operator[](size_t i) const
	{
		return _vmon[i];
	}
protected:
	vector<monomial> _vmon;
	int _size;
};

polynomial operator+(const monomial& A, const monomial& B);
polynomial operator-(const monomial& A, const monomial& B);
monomial operator*(const monomial& A, const monomial& B);

#endif
