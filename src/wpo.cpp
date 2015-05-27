#include "kernel.hpp"
#include <iostream>

int main()
{
	monomial m1;
	m1.resize(4);
	m1[0] = 1;
	monomial m2;
	m2.resize(4);
	m2[0] = 3;
	m2[1] = 1;
	monomial m3;
	m3.resize(4);
	m3[0] = 5;
	m3[2] = 1;
	monomial m4;
	m4.resize(4);
	m4[0] = 7;
	m4[3] = 1;
	polynomial p;
	p += m1;
	p -= m2;
	p += m3;
	p -= m4;
	set<monomial> scok;
	set<polynomial> sk;
	find_kernels(vector<polynomial>{p}, scok, sk);
	return 0;
}
