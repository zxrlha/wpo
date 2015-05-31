#ifndef WPO_KCM_HPP
#define WPO_KCM_HPP 1

#include <set>
#include <map>
#include "expression.hpp"

using std::set;
using std::map;

class kcm
{
public:
	kcm(const vector<map<monomial, polynomial>>& vkmap, const vector<polynomial>& vP);
	bool generate_best_rectangle(vector<int>& row, vector<int>& column);
	const monomial& row(int i) const { return _vcok[i]; };
	const monomial& column(int i) const { return _vk[i]; };
	int index(int i) const { return _icok[i]; };
protected:
	int value_of_best_rectangle(int ri, int ci, vector<int>& row, vector<int>& column);
	int value_of_best_rectangle(vector<int>& row, vector<int>& column, set<int>& posi_rows, set<int>& posi_columns);
	int value_of_prime_rectangle(vector<int>& row, vector<int>& column);

	vector<monomial> _vcok;
	vector<int> _icok;
	vector<int> _vMR;//M(R_i)
	vector<monomial> _vk;
	vector<int> _vMC;//M(C_i)
	vector<vector<int> > _mat;
};

#endif
