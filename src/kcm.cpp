#include <algorithm>
#include "kcm.hpp"
#include <iostream>
//#include <wat/timer.hpp>

kcm::kcm(vector<vector<pair<monomial, polynomial>>>&& vkmap)
{
    vector<polynomial> tvk;
    //co-kernels: _vcok, _icok
    for (int i = 0; i < vkmap.size(); ++i)
    {
        for (auto& pr : vkmap[i])
        {
            _vcok.push_back(std::move(pr.first));
            _icok.push_back(i);
            tvk.push_back(std::move(pr.second));
        }
    }
    //kernels
    std::set<monomial> skm;
    for (int i = 0; i < tvk.size(); ++i)
    {
        for (int j = 0; j < tvk[i].size(); ++j)
        {
            skm.insert(tvk[i][j]);
        }
    }
    _vk.resize(skm.size());
    std::move(skm.begin(), skm.end(), _vk.begin());
    //matrix
    _mat_nonzero_cols.resize(_vcok.size());
    _mat_nonzero_rows.resize(_vk.size());
    //_mat.resize(_vcok.size(), vector<bool>(_vk.size(), false));
    for (int i = 0; i < _vcok.size(); ++i)
    {
        for (int j = 0; j < tvk[i].size(); ++j)
        {
            auto it = std::lower_bound(_vk.begin(), _vk.end(), tvk[i][j]);
            int index = it - _vk.begin();
            _mat_nonzero_cols[i].push_back(index);
            _mat_nonzero_rows[index].push_back(i);
            //_mat[i][index] = true;
        }
    }
    for (int i = 0; i < _vcok.size(); ++i)
    {
        std::sort(_mat_nonzero_cols[i].begin(), _mat_nonzero_cols[i].end());
        auto last = std::unique(_mat_nonzero_cols[i].begin(), _mat_nonzero_cols[i].end());
        _mat_nonzero_cols[i].erase(last, _mat_nonzero_cols[i].end());
    }
    for (int j = 0; j < _vk.size(); ++j)
    {
        std::sort(_mat_nonzero_rows[j].begin(), _mat_nonzero_rows[j].end());
        auto last = std::unique(_mat_nonzero_rows[j].begin(), _mat_nonzero_rows[j].end());
        _mat_nonzero_rows[j].erase(last, _mat_nonzero_rows[j].end());
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

int kcm::generate_best_rectangle(vector<int>& row, vector<int>& column)
{
    _bv = 0;
    _br.clear();
    _bc.clear();
    for (int i = 0; i < _vcok.size(); ++i)
    {
        for (int in_j = 0; in_j < _mat_nonzero_cols[i].size(); ++in_j)
        {
            int j = _mat_nonzero_cols[i][in_j];
            vector<int> posi_columns;
            for (int in_ci2 = 0; in_ci2 < in_j; ++in_ci2)
            {
                int ci2 = _mat_nonzero_cols[i][in_ci2];
                generate_best_rectangle(i, j, ci2, _mat_nonzero_rows[ci2], posi_columns);
                posi_columns.push_back(ci2);
            }
        }
    }
    if (_bv == 0) { return -1; }
    if (_br.size() == 1 && _bc.size() == 1) { return -1; }
    row = _br;
    column = _bc;
    return _bv;
}

namespace
{
    vector<int> vector_intersection(const vector<int>& v1, const vector<int>& v2, int up)
    {
        vector<int> res;
        int i1 = 0;
        int i2 = 0;
        while (i1 != v1.size() && i2 != v2.size() && v1[i1] < up && v2[i2] < up)
        {
            if (v1[i1] < v2[i2])
            {
                ++i1;
            }
            else if (v2[i2] < v1[i1])
            {
                ++i2;
            }
            else
            {
                res.push_back(v1[i1]);
                ++i1;
                ++i2;
            }
        }
        return std::move(res);
    }
}

void kcm::generate_best_rectangle(int ri, int ci1, int ci2, const vector<int>& aposi_rows, const vector<int>& posi_columns)
{
    vector<int> posi_rows(vector_intersection(aposi_rows, _mat_nonzero_rows[ci1], ri));
    _prs = posi_rows.size();
    _pcs = posi_columns.size();
    vector<int> row{ri};
    vector<int> column{ci1, ci2};
    _sumMR = _vMR[ri];
    _sumMC = _vMC[ci1] + _vMC[ci2];
    generate_best_rectangle_11(row, column, posi_rows, posi_columns);
}

//00: _prs == 0, _pcs == 0, prime rectangle
void kcm::generate_best_rectangle_00(vector<int>& row, vector<int>& column)
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
void kcm::generate_best_rectangle_01(vector<int>& row, vector<int>& column, const vector<int>& posi_columns)
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
    generate_best_rectangle_00(row, column);
    column.resize(cs);
    _sumMC = osummc;
}

//10:no possible row, but possible columns, add them all
void kcm::generate_best_rectangle_10(vector<int>& row, vector<int>& column, const vector<int>& posi_rows)
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
    generate_best_rectangle_00(row, column);
    row.resize(rs);
    _sumMR = osummr;
}

//11: possible row and possible column
void kcm::generate_best_rectangle_11(vector<int>& row, vector<int>& column, const vector<int>& posi_rows, const vector<int>& posi_columns)
{
    if (_prs == 0)
    {
        generate_best_rectangle_01(row, column, posi_columns);
    }
    else if (_pcs == 0)
    {
        generate_best_rectangle_10(row, column, posi_rows);
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

void kcm::generate_best_rectangle_11_row(vector<int>& row, vector<int>& column, const vector<int>& posi_rows, const vector<int>& posi_columns)
{
    int rb = row.back();
    _prs = 0;
    int opcs = _pcs;
    //add possible row
    for (auto it = posi_rows.begin(); it != posi_rows.end() && *it < rb; ++it, ++_prs)
    {
        int i = *it;
        row.push_back(i);
        _sumMR += _vMR[i];
        //build new posi_columns
        vector<int> new_posi_columns(vector_intersection(posi_columns, _mat_nonzero_cols[i], column.back()));
        _pcs = new_posi_columns.size();
        generate_best_rectangle_11(row, column, posi_rows, new_posi_columns);
        //change row back
        row.erase(row.end() - 1);
        _sumMR -= _vMR[i];
    }
    //change _pcs back
    _pcs = opcs;
}

void kcm::generate_best_rectangle_11_column(vector<int>& row, vector<int>& column, const vector<int>& posi_rows, const vector<int>& posi_columns)
{
    int cb = column.back();
    _pcs = 0;
    int oprs = _prs;
    //add possible column
    for (auto it = posi_columns.begin(); it != posi_columns.end() && *it < cb; ++it, ++_pcs)
    {
        int i = *it;
        column.push_back(i);
        _sumMC += _vMC[i];
        //build new posi_rows
        vector<int> new_posi_rows(vector_intersection(posi_rows, _mat_nonzero_rows[i], row.back()));
        _prs = new_posi_rows.size();
        generate_best_rectangle_11(row, column, new_posi_rows, posi_columns);
        //change column back
        column.erase(column.end() - 1);
        _sumMC -= _vMC[i];
    }
    _prs = oprs;
}
int kcm::value_of_prime_rectangle(vector<int>& row, vector<int>& column)
{
    int C = column.size();
    int R = row.size();
    return (C - 1) * (_sumMR + R) + (R - 1) * (_sumMC);
}

int kcm::value_diff_nr(vector<int>& row, vector<int>& column, int nrow)
{
    int C = column.size();
    return (C - 1) * (1 + _vMR[nrow]) + _sumMC;
}

int kcm::value_diff_nc(vector<int>& row, vector<int>& column, int ncolumn)
{
    int R = row.size();
    return (_sumMR + R) + (R - 1) * _vMC[ncolumn];
}
