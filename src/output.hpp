#ifndef WPO_OUTPUT_HPP
#define WPO_OUTPUT_HPP 11

#include <vector>
#include <iostream>
#include "expression.hpp"

using std::ostream;

string output_tmp_name(int i);
//void clean();
//In vorder, negative number means -1-index in vfunc, while positive number means index in vP
void reorder(vector<vector<int>>& vorder);
void rename(const vector<vector<int>>& vorder, vector<int>& max);

#endif
