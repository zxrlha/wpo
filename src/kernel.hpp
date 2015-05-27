#ifndef WPO_KERNEL_HPP
#define WPO_KERNEL_HPP 1

#include "expression.hpp"
#include <set>

using std::set;

void find_kernels(const vector<polynomial>& P, set<monomial>& sco_kernels, set<polynomial>& skernels);

#endif
