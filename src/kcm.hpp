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
	void generate_best_rectangle(int ri, int ci1, int ci2);
	void generate_best_rectangle_11(vector<int>& row, vector<int>& column, set<int>& posi_rows, set<int>& posi_columns);
	void generate_best_rectangle_11_row(vector<int>& row, vector<int>& column, set<int>& posi_rows, set<int>& posi_columns);
	void generate_best_rectangle_11_column(vector<int>& row, vector<int>& column, set<int>& posi_rows, set<int>& posi_columns);
	void generate_best_rectangle_10(vector<int>& row, vector<int>& column, set<int>& posi_rows, set<int>& posi_columns);
	void generate_best_rectangle_01(vector<int>& row, vector<int>& column, set<int>& posi_rows, set<int>& posi_columns);
	void generate_best_rectangle_00(vector<int>& row, vector<int>& column, set<int>& posi_rows, set<int>& posi_columns);
	bool is_prime(int ri, int ci, const vector<int>& posi_rows, const vector<int>& posi_columns);
	int value_of_prime_rectangle(vector<int>& row, vector<int>& column);
	int value_diff_nr(vector<int>& row, vector<int>& column, int nr);
	int value_diff_nc(vector<int>& row, vector<int>& column, int nc);

	vector<monomial> _vcok;
	vector<int> _icok;
	vector<int> _vMR;//M(R_i)
	vector<monomial> _vk;
	vector<int> _vMC;//M(C_i)
	vector<vector<int> > _mat;

	vector<int> _br;
	vector<int> _bc;
	int _bv;

	//tmp variable
	int _sumMR;
	int _sumMC;
	int _prs;
	int _pcs;
};

#endif
