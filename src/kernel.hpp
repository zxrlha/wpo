#ifndef WPO_KERNEL_HPP
#define WPO_KERNEL_HPP 1

#include "expression.hpp"
#include <map>

using std::map;

void find_kernels(const polynomial& P, map<monomial, polynomial>& kernelmap);

#endif
