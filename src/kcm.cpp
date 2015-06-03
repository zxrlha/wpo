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
					if (i == 248) exit(0);
				}
			}
		}
	}

	//std::cout<<max<<std::endl;
	if (_bv == 0) return false;
	if (_br.size() == 1 && _bc.size() == 1) return false;
	row = _br;
	column = _bc;
	return true;
}

void kcm::generate_best_rectangle(int ri, int ci1, int ci2)
{
	vector<int> posi_rows(_vcok.size());
	vector<int> posi_columns(_vk.size());
	for (int i = 0; i < ri; ++i)
	{
		if (_mat[i][ci1] != 0 && _mat[i][ci2] != 0)//possible rows
		{
			posi_rows[i] = 1;
		}
	}
	for (int i = 0; i < ci2; ++i)
	{
		if (_mat[ri][i] != 0)//possible columns
		{
			posi_columns[i] = 1;
		}
	}
	vector<int> row{ri};
	vector<int> column{ci1, ci2};
	generate_best_rectangle(row, column, posi_rows, posi_columns);
}

//row contain rows included in rectangle
//column contain columns included in rectangle
//posi_rows and posi_columns are allowed to included row/column
void kcm::generate_best_rectangle(vector<int>& row, vector<int>& column, vector<int>& posi_rows, vector<int>& posi_columns)
{
	bool is_prime = true;
	for (int i = 0; i < row.back(); ++i)
		if (posi_rows[i] != 0)
		{
			is_prime = false;
			break;
		}
	if (is_prime)
		for (int i = 0; i < column.back(); ++i)
			if (posi_columns[i] != 0)
			{
				is_prime = false;
				break;
			}
	if (is_prime)//it is prime rectangle now
	{
		int v = value_of_prime_rectangle(row, column);
		if (v >= _bv)
		{
			_br = row;
			_bc = column;
			_bv = v;
		}
		return;
	}
	//add possible row
	for (int i = 0; i < row.back(); ++i)
	{
		if (posi_rows[i] == 0) continue;
		row.push_back(i);
		//build new posi_rows and posi_columns
		set<int> trpc;
		//remove columns which is not possible since i is included
		for (int j = 0; j < column.back(); ++j)
		{
			if (posi_columns[j] == 1 && _mat[i][j] != 1)
			{
				posi_columns[j] = 0;
				trpc.insert(j);
			}
		}
		generate_best_rectangle(row, column, posi_rows, posi_columns);
		//add back
		for (auto ci : trpc)
		{
			posi_columns[ci] = 1;
		}
		row.erase(row.end() - 1);
	}
	generate_best_rectangle(row, column, posi_columns);
}

void kcm::generate_best_rectangle(vector<int>& row, vector<int>& column, vector<int>& posi_columns)
{
	int cs = column.size();
	int tb = column.back();
	for (int i = 0; i < tb; ++i)
	{
		if (posi_columns[i] == 0) continue;
		column.push_back(i);
	}
	int v = value_of_prime_rectangle(row, column);
	if (v >= _bv)
	{
		_br = row;
		_bc = column;
		_bv = v;
	}
	column.resize(cs);
}
int kcm::value_of_prime_rectangle(vector<int>& row, vector<int>& column)
{
	int C = column.size();
	int R = row.size();
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
	//std::cout<<3<<C<<" "<<R<<" "<<sumMR<<" "<<sumMC<<std::endl;
	return (C - 1) * (sumMR + R) + (R - 1) * (sumMC);
}
