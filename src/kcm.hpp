#ifndef WPO_KCM_HPP
#define WPO_KCM_HPP 1

#include <set>
#include <map>
#include "expression.hpp"

using std::set;
using std::map;
using std::pair;

class kcm
{
public:
	kcm(const vector<vector<pair<monomial, polynomial>>>& vkmap, const vector<polynomial>& vP);
	bool generate_best_rectangle(vector<int>& row, vector<int>& column);
	const monomial& row(int i) const { return _vcok[i]; };
	const monomial& column(int i) const { return _vk[i]; };
	int index(int i) const { return _icok[i]; };
protected:
	void generate_best_rectangle(int ri, int ci, vector<int>& row, vector<int>& column);
	void generate_best_rectangle(vector<int>& row, vector<int>& column, vector<int>& posi_rows, vector<int>& posi_columns);
	int value_of_prime_rectangle(vector<int>& row, vector<int>& column);

	vector<monomial> _vcok;
	vector<int> _icok;
	vector<int> _vMR;//M(R_i)
	vector<monomial> _vk;
	vector<int> _vMC;//M(C_i)
	vector<vector<int> > _mat;

	vector<int> _br;
	vector<int> _bc;
	int _bv;
};

#endif
