#include <algorithm>
#include "kcm.hpp"
#include <iostream>

kcm::kcm(const vector<vector<pair<monomial, polynomial>>>& vkmap, const vector<polynomial>& vP)
{
	vector<polynomial> tvk;
	//co-kernels
	for (int i = 0; i < vkmap.size(); ++i)
	{
		for (auto pr : vkmap[i])
		{
			_vcok.push_back(pr.first);
			_icok.push_back(i);
			tvk.push_back(pr.second);
		}
	}
	//kernels
	std::set<monomial> skm;
	for (int i = 0; i < vkmap.size(); ++i)
	{
		for (auto pr : vkmap[i])
		{
			for (int j = 0; j < pr.second.number(); ++j)
			{
				skm.insert(pr.second[j]);
			}
		}
	}
	_vk.resize(skm.size());
	std::copy(skm.begin(), skm.end(), _vk.begin());

	//matrix
	_mat.resize(_vcok.size());
	for (auto& v : _mat)
	{
		v.resize(_vk.size());
	}

	for (int i = 0; i < _vcok.size(); ++i)
	{
		for (int j = 0; j < _vk.size(); ++j)
		{
			monomial m = _vk[j];
			if (tvk[i].contain(m))
				_mat[i][j] = 1;
			else
				_mat[i][j] = 0;
		}
	}
	for (int i = 0; i < _vcok.size(); ++i)
	{
		_vMR.push_back(_vcok[i].multiplication_number() - 1);
	}
	for (int i = 0; i < _vk.size(); ++i)
	{
		_vMC.push_back(_vk[i].multiplication_number() - 1);
	}
}

bool kcm::generate_best_rectangle(vector<int>& row, vector<int>& column)
{
	_bv = 0;
	_br.clear();
	_bc.clear();

	int start = 0;

	if (_vcok.size() == 0) return false;
	if (_vcok.size() <= start) return false;
	for (int i = start; i < _vcok.size(); ++i)
	{
		for (int j = 0; j < _vk.size(); ++j)
		{
			if (_mat[i][j] == 1)
			{
				for (int ci2 = 0; ci2 < j; ++ci2)
				{
					if (_mat[i][ci2] == 1)
						generate_best_rectangle(i, j, ci2);
					//if (i == 248) exit(0);
				}
			}
		}
	}

	if (_bv == 0) return false;
	if (_br.size() == 1 && _bc.size() == 1) return false;
	row = _br;
	column = _bc;
	return true;
}

void kcm::generate_best_rectangle(int ri, int ci1, int ci2)
{
	set<int> posi_rows;
	set<int> posi_columns;
	for (int i = 0; i < ri; ++i)
	{
		if (_mat[i][ci1] != 0 && _mat[i][ci2] != 0)//possible rows
		{
			posi_rows.insert(i);
		}
	}
	_prs = posi_rows.size();
	for (int i = 0; i < ci2; ++i)
	{
		if (_mat[ri][i] != 0)//possible columns
		{
			posi_columns.insert(i);
		}
	}
	_pcs = posi_columns.size();
	vector<int> row{ri};
	vector<int> column{ci1, ci2};
	_sumMR = _vMR[ri];
	_sumMC = _vMC[ci1] + _vMC[ci2];
	generate_best_rectangle_11(row, column, posi_rows, posi_columns);
}

//00: _prs == 0,_pcs == 0, prime rectangle
void kcm::generate_best_rectangle_00(vector<int>& row, vector<int>& column, set<int>& posi_rows, set<int>& posi_columns)
{
	int v = value_of_prime_rectangle(row, column);
	if (v >= _bv)
	{
		_br = row;
		_bc = column;
		_bv = v;
	}
}


//01:no possible row, but possible columns, add them all
void kcm::generate_best_rectangle_01(vector<int>& row, vector<int>& column, set<int>& posi_rows, set<int>& posi_columns)
{
	int cs = column.size();
	int cb = column.back();
	int osummc = _sumMC;
	for (auto it = posi_columns.begin(); it != posi_columns.end() && *it < cb; ++it)
	{
		int i = *it;
		if (value_diff_nc(row, column, i) > 0)
		{
		column.push_back(i);
		_sumMC += _vMC[i];
		}
	}
	generate_best_rectangle_00(row, column, posi_rows, posi_columns);
	column.resize(cs);
	_sumMC = osummc;
}

//10:no possible row, but possible columns, add them all
void kcm::generate_best_rectangle_10(vector<int>& row, vector<int>& column, set<int>& posi_rows, set<int>& posi_columns)
{
	int rs = row.size();
	int rb = row.back();
	int osummr = _sumMR;
	for (auto it = posi_rows.begin(); it != posi_rows.end() && *it < rb; ++it)
	{
		int i = *it;
		if (value_diff_nr(row, column, i) > 0)
		{
		row.push_back(i);
		_sumMR += _vMR[i];
		}
	}
	generate_best_rectangle_00(row, column, posi_rows, posi_columns);
	row.resize(rs);
	_sumMR = osummr;
}

//11: possible row and possible column
void kcm::generate_best_rectangle_11(vector<int>& row, vector<int>& column, set<int>& posi_rows, set<int>& posi_columns)
{
	if (_prs == 0)
	{
		generate_best_rectangle_01(row, column, posi_rows, posi_columns);
	}
	else if (_pcs == 0)
	{
		generate_best_rectangle_10(row, column, posi_rows, posi_columns);
	}
	if (_prs < _pcs)
	{
		generate_best_rectangle_11_row(row, column, posi_rows, posi_columns);
	}
	else
	{
		generate_best_rectangle_11_column(row, column, posi_rows, posi_columns);
	}
}

void kcm::generate_best_rectangle_11_row(vector<int>& row, vector<int>& column, set<int>& posi_rows, set<int>& posi_columns)
{
	//std::cout << "ROW:" << _prs << " " << _pcs << std::endl;
	int rb = row.back();
	//add possible row
	_prs = 0;
	for (auto it = posi_rows.begin(); it != posi_rows.end() && *it < rb; ++it, ++_prs)
	{
		int i = *it;
		row.push_back(i);
		_sumMR += _vMR[i];
		//build new posi_columns
		set<int> trpc;
		//remove columns which is not possible since i is included
		for (auto j : posi_columns)
		{
			if (_mat[i][j] != 1)
			{
				trpc.insert(j);
			}
		}
		for (auto j : trpc)
		{
			posi_columns.erase(j);
		}
		_pcs -= trpc.size();
		generate_best_rectangle_11(row, column, posi_rows, posi_columns);
		//add back
		for (auto j : trpc)
		{
			posi_columns.insert(j);
		}
		_pcs += trpc.size();
		row.erase(row.end() - 1);
		_sumMR -= _vMR[i];
	}
}

void kcm::generate_best_rectangle_11_column(vector<int>& row, vector<int>& column, set<int>& posi_rows, set<int>& posi_columns)
{
	//std::cout << "COLUMN" << _prs << " " << _pcs << std::endl;
	int cb = column.back();
	//add possible column
	_pcs = 0;
	for (auto it = posi_columns.begin(); it != posi_columns.end() && *it < cb; ++it, ++_pcs)
	{
		int i = *it;
		column.push_back(i);
		_sumMC += _vMC[i];
		//build new posi_rows
		set<int> trpr;
		//remove rows which is not possible since i is included
		for (auto j : posi_rows)
		{
			if (_mat[j][i] != 1)
			{
				trpr.insert(j);
			}
		}
		for (auto j : trpr)
		{
			posi_rows.erase(j);
		}
		_prs -= trpr.size();
		generate_best_rectangle_11(row, column, posi_rows, posi_columns);
		//add back
		for (auto j : trpr)
		{
			posi_rows.insert(j);
		}
		_prs += trpr.size();
		column.erase(column.end() - 1);
		_sumMC -= _vMC[i];
	}
}
int kcm::value_of_prime_rectangle(vector<int>& row, vector<int>& column)
{
	int C = column.size();
	int R = row.size();
	/*
	int sumMR = 0;
	int sumMC = 0;
	for (int i = 0; i < row.size(); ++i)
	{
	    sumMR += _vMR[row[i]];
	}
	for (int i = 0; i < column.size(); ++i)
	{
	    sumMC += _vMC[column[i]];
	}
	*/
	//std::cout<<3<<C<<" "<<R<<" "<<sumMR<<" "<<sumMC<<std::endl;
	return (C - 1) * (_sumMR + R) + (R - 1) * (_sumMC);
}

int kcm::value_diff_nr(vector<int>& row, vector<int>& column, int nrow)
{
	int C = column.size();
	return (C-1)*(1+_vMR[nrow])+_sumMC;
}

int kcm::value_diff_nc(vector<int>& row, vector<int>& column, int ncolumn)
{
	int R = row.size();
	return (_sumMR+R)+(R-1)*_vMC[ncolumn];
}
