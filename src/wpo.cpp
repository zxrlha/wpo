#include "kernel.hpp"
#include <iostream>
#include "extraction.hpp"
#include "kcm.hpp"
#include "literal.hpp"

int main()
{
	polynomial P;
	monomial m;
	m.resize(2);
	//u,v,z,a,b,w,q,4,6
	literal_append("u");
	literal_append("v");
	/*
	literal_append("z");
	literal_append("a");
	literal_append("b");
	literal_append("w");
	literal_append("q");
	literal_append("4");
	literal_append("6");
	*/
	/*
	vector<int> coef1{4,0,1,0,0,0,0,0,0};
	vector<int> coef2{3,1,0,1,0,0,0,1,0};
	vector<int> coef3{2,2,0,0,1,0,0,0,1};
	vector<int> coef4{1,3,0,0,0,1,0,1,0};
	vector<int> coef5{0,4,0,0,0,0,1,0,0};
	*/
	vector<int> coef1{4,0};
	vector<int> coef2{3,1};
	vector<int> coef3{1,3};
	vector<int> coef4{0,4};
	vector<int> coef[4]={coef1,coef2,coef3,coef4};
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			m[j] = coef[i][j];
		}
		P += m;
	}
	P.name() = "P";
	vector<polynomial> vP{P};
	find_kernel_intersections(vP);
	find_cube_intersections(vP);
	for (int i = 0; i < vP.size(); ++i)
	{
		std::cout<<vP[i]<<std::endl;
	}

	return 0;
}
