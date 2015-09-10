#ifndef WPO_EXTRACTION_HPP
#define WPO_EXTRACTION_HPP 1

#include "expression.hpp"

void substitution(vector<polynomial>& vP, const polynomial& s);
void fr_find_kernel_intersections(vector<polynomial>& vP);
void find_kernel_intersections(vector<polynomial>& vP);
void find_cube_intersections(vector<polynomial>& vP);

#endif
