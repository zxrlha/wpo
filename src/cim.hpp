#ifndef WPO_CIM_HPP
#define WPO_CIM_HPP 1

#include <set>
#include "expression.hpp"

using std::set;

class cim
{
public:
	explicit cim(const vector<polynomial>& vP);
	bool generate_best_rectangle(vector<int>& row, monomial& m);
protected:
	void generate_best_rectangle(int ri, vector<int>& row, const monomial& m);
	int value_of_prime_rectangle(vector<int>& row, const monomial& m);
	vector<monomial> _mat;

	vector<int> _br;
	monomial _bm;
	int _bv;
};

#endif
