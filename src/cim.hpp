#ifndef WPO_CIM_HPP
#define WPO_CIM_HPP 1

#include <set>
#include <list>
#include "expression.hpp"

using std::set;
using std::list;
using std::pair;

class cim
{
public:
	explicit cim(const vector<polynomial>& vP);
	bool generate_best_rectangle(monomial& m);
protected:
	void generate_best_rectangle(int ri);
	void generate_best_rectangle(int rowsize, const monomial& m, vector<int>& posi);
	int value_of_prime_rectangle(int rowsize, const monomial& m);
	vector<monomial> _mat;

	int _bs;
	monomial _bm;
	int _bv;
};

#endif
