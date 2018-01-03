#ifndef WPO_KCM_HPP
#define WPO_KCM_HPP 1

#include <set>
#include <map>
#include <vector>
#include "expression.hpp"

using std::set;
using std::map;
using std::pair;
using std::vector;

class kcm
{
public:
	kcm(vector<vector<pair<monomial, polynomial>>>&& vkmap);
	int generate_best_rectangle(vector<int>& row, vector<int>& column);
	const monomial& row(int i) const { return _vcok[i]; };
	const monomial& column(int i) const { return _vk[i]; };
	int index(int i) const { return _icok[i]; };
protected:
	void generate_best_rectangle(int ri, int ci, const vector<int>& aposi_rows, const vector<int>& posi_columns);
	void generate_best_rectangle_11(vector<int>& row, vector<int>& column, const vector<int>& posi_rows, const vector<int>& posi_columns);
	void generate_best_rectangle_11_row(vector<int>& row, vector<int>& column, const vector<int>& posi_rows, const vector<int>& posi_columns);
	void generate_best_rectangle_11_column(vector<int>& row, vector<int>& column, const vector<int>& posi_rows, const vector<int>& posi_columns);
	void generate_best_rectangle_10(vector<int>& row, vector<int>& column, const vector<int>& posi_rows);
	void generate_best_rectangle_01(const vector<int>& row, const vector<int>& column, const vector<int>& posi_columns);
	void generate_best_rectangle_00(vector<int>& row, vector<int>& column);
	bool is_prime(int ri, int ci, const vector<int>& posi_rows, const vector<int>& posi_columns);
	int value_of_prime_rectangle(int C, int R, int sMC, int sMR);
	int value_diff_nr(vector<int>& row, vector<int>& column, int nr);
	int value_diff_nc(vector<int>& row, vector<int>& column, int nc);

	vector<monomial> _vcok;
	vector<int> _icok;
	vector<int> _vMR;//M(R_i)
	vector<monomial> _vk;
	vector<int> _vMC;//M(C_i)
    //The matrix is represented by two vectors:
    vector<vector<int> > _mat_nonzero_rows;
    vector<vector<int> > _mat_nonzero_cols;
    //if _mat[i][j] !=0, then j \in _mat_nonzero_cols[i], i\in _mat_nonzero_rows[j]
	//vector<vector<bool> > _mat;

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
