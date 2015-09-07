#ifndef WPO_KERNEL_HPP
#define WPO_KERNEL_HPP 1

#include "expression.hpp"
#include <map>

using std::map;
using std::pair;

void find_kernels(const polynomial& P, vector<pair<monomial, polynomial>>& kernelmap);
void fr_find_kernels(const polynomial& P, monomial& bk, polynomial& bcok, int& bv);

#endif
