#ifndef WPO_OUTPUT_HPP
#define WPO_OUTPUT_HPP 11

#include <vector>
#include <iostream>
#include "expression.hpp"

using std::ostream;

string output_tmp_name(int i);
void reorder(vector<polynomial>& vP);
int rename(vector<polynomial>& vP);

#endif
