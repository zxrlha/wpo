#include <algorithm>
#include <iostream>
#include "kernel.hpp"
#include "literal.hpp"

using std::make_pair;

void kernels(int i, const polynomial& P, const monomial& d, vector<pair<monomial, polynomial>>& kernelmap)
{
	for (int j = i; j < literal_size(); ++j)
	{
		//std::cout<<"kernels:"<<j<<std::endl;
		int times = 0;
		for (int k = 0; k < P.size(); ++k)
		{
			if (P[k].getpow(j) != 0)
			{
				++times;
			}
		}
		if (times > 1)
		{
			monomial Lj(j);
			polynomial Ft = P / Lj;
			monomial C = Ft.gcd();
			bool cflag = true;
			for (int k = 0; k < C.size(); ++k)
			{
				if (C.lit(k) < j)
				{
					cflag = false;
					break;
				}
			}
			if (cflag)
			{
				polynomial FI = Ft / C;//kernel
				monomial DI = d * C * Lj;//co-kernel
				//special for if FI=1+...
				/*
				if (FI.contain(monomial()))
				{
					for (int l = 0; l < C.size(); ++l)
					{
						polynomial NFI = FI * monomial(C.lit(l));
						monomial NDI = DI;
						NDI /= monomial(C.lit(l));
						std::cout<<NDI<<" "<<NFI<<std::endl;
						kernelmap.push_back(make_pair(NDI, NFI));
						kernels(j, NFI, NDI, kernelmap);
					}
				}
				*/
				kernelmap.push_back(make_pair(DI, FI));
				kernels(j, FI, DI, kernelmap);
			}
		}
	}
}

void find_kernels(const polynomial& P, vector<pair<monomial, polynomial>>& kernelmap)
{
	kernelmap.clear();
	kernels(0, P, monomial(), kernelmap);
	if (P.gcd() == monomial())
	{
		kernelmap.push_back(make_pair(monomial(), P));
	}
}

//The fr_kernels part are part of initial fastrun stategy
//Now they are deprecated, but we still keep these code in case.
int fr_value(const monomial& bk, const polynomial& bcok)
{
	return (bcok.size() - 1) * bk.multiplication_number();
}

void fr_kernels(int i, const polynomial& P, const monomial& d, monomial& bk, polynomial& bcok, int& bv)
{
	for (int j = i; j < literal_size(); ++j)
	{
		int times = 0;
		for (int k = 0; k < P.size(); ++k)
		{
			if (P[k].getpow(j) != 0)
			{
				++times;
			}
		}
		if (times > 1)
		{
			monomial Lj(j);
			polynomial Ft = P / Lj;
			monomial C = Ft.gcd();
			bool cflag = true;
			for (int k = 0; k < C.size(); ++k)
			{
				if (C.lit(k) < j)
				{
					cflag = false;
					break;
				}
			}
			if (cflag)
			{
				polynomial FI = Ft / C;//kernel
				monomial DI = d * C * Lj;//co-kernel
				int v = fr_value(DI, FI);
				if (v > bv)
				{
					bv = v;
					bk = DI;
					bcok = FI;
				}
				fr_kernels(j, FI, DI, bk, bcok, bv);
			}
		}
	}
}

void fr_find_kernels(const polynomial& P, monomial& bk, polynomial& bcok, int& bv)
{
	fr_kernels(0, P, monomial(), bk, bcok, bv);
}
