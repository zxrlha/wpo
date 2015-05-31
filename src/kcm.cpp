#include <algorithm>
#include "kcm.hpp"
#include <iostream>

kcm::kcm(const vector<map<monomial, polynomial>>& vkmap, const vector<polynomial>& vP)
{
	vector<polynomial> tvk;
	//co-kernels
	for (int i = 0; i < vkmap.size(); ++i)
	{
		for (auto pr: vkmap[i])
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
		for (auto pr:  vkmap[i])
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
	int max = 0;
	int start = 0;
	if (_vcok.size() == 0) return false;
	if (_vcok[0] == monomial()) start = 1;
	if (_vcok.size() <= start) return false;
	for (int i = start; i < _vcok.size(); ++i)
	{
		for (int j = 0; j < _vk.size(); ++j)
		{
			if (_mat[i][j] == 1)
			{
				vector<int> vr;
				vector<int> vc;
				int v = value_of_best_rectangle(i, j, vr, vc);
				if (v > max)
				{
					max = v;
					row = vr;
					column = vc;
				}
			}
		}
	}
	//std::cout<<max<<std::endl;
	if (max == 0) return false;
	if (row.size() == 1 && column.size() == 1) return false;
	return true;
}

int kcm::value_of_best_rectangle(int ri, int ci, vector<int>& row, vector<int>& column)
{
	set<int> posi_rows;
	set<int> posi_columns;
	for (int i = 0; i < ri; ++i)
	{
		if (_mat[i][ci] != 0)//possible rows
		{
			posi_rows.insert(i);
		}
	}
	for (int i = 0; i < ci; ++i)
	{
		if (_mat[ri][i] != 0)//possible columns
		{
			posi_columns.insert(i);
		}
	}
	row.push_back(ri);
	column.push_back(ci);
	return value_of_best_rectangle(row, column, posi_rows, posi_columns);
}

//row contain rows included in rectangle
//column contain columns included in rectangle
//posi_rows and posi_columns are allowed to included row/column
int kcm::value_of_best_rectangle(vector<int>& row, vector<int>& column, set<int>& posi_rows, set<int>& posi_columns)
{
	if (posi_rows.size() == 0 && posi_columns.size() == 0)//it is prime rectangle know
	{
		return value_of_prime_rectangle(row, column);
	}
	int max = 0;
	vector<int> bestvr;
	vector<int> bestvc;
	if (posi_rows.size() != 0)//now we can added one row
	{
		for (auto i : posi_rows)
		{
			vector<int> tr(row);
			vector<int> tc(column);
			tr.push_back(i);//include this row
			//build new posi_rows and posi_columns
			set<int> tpr(posi_rows);
			set<int> tpc(posi_columns);
			//remove rows bigger than i
			for (auto it = tpr.begin(); it != tpr.end();)
			{
				if (*it >= i) it = tpr.erase(it);
				else ++it;
			}
			//remove columns which is not possible since i is included
			for (auto it = tpc.begin(); it != tpc.end();)
			{
				if (_mat[i][*it] != 1)
				{
					it = tpc.erase(it);
				}
				else ++it;
			}
			int value = value_of_best_rectangle(tr, tc, tpr, tpc);
			if (value > max)
			{
				max = value;
				bestvr = tr;
				bestvc = tc;
			}
		}
	}
	//now no more rows
	set<int> nrow;
	for (auto i : posi_columns)
	{
		vector<int> tr(row);
		vector<int> tc(column);
		tc.push_back(i);//include this column
		//build new posi_columns
		set<int> tpc(posi_columns);
		//remove columns bigger than i
		for (auto it = tpc.begin(); it != tpc.end();)
		{
			if (*it >= i) it = tpc.erase(it);
			else ++it;
		}
		int value = value_of_best_rectangle(tr, tc, nrow, tpc);
		if (value > max)
		{
			max = value;
			bestvr = tr;
			bestvc = tc;
		}
	}
	row = bestvr;
	column = bestvc;
	return max;
}

int kcm::value_of_prime_rectangle(vector<int>& row, vector<int>& column)
{
	int C = column.size();
	int R = row.size();
	int nsumMR = 0;
	int sumMC = 0;
	for (int i = 0; i < row.size(); ++i)
	{
		nsumMR += (_vMR[row[i]] + 1) * (C - 1);
	}
	for (int i = 0; i < column.size(); ++i)
	{
		sumMC += _vMC[column[i]];
	}
	return nsumMR + (R - 1) * (sumMC);
}
