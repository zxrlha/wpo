#include <algorithm>
#include "kernel.hpp"

using std::make_pair;

void kernels(int i, const polynomial& P, const monomial& d, map<monomial, polynomial>& kernelmap)
{
	for (int j = i; j < P.size(); ++j)
	{
		int times = 0;
		for (int k = 0; k < P.number(); ++k)
		{
			if (P[k][j] != 0) ++times;
		}
		if (times > 1)
		{
			monomial Lj;
			Lj.resize(P.size());
			Lj[j] = 1;
			polynomial Ft = P / Lj;
			monomial C = Ft.gcd();
			bool cflag = true;
			for (int k = 0; k < j; ++k)
			{
				if (C[k] != 0)
				{
					cflag = false;
					break;
				}
			}
			if (cflag)
			{
				polynomial FI = Ft / C;//kernel
				monomial DI = d * C * Lj;//co-kernel
				kernelmap.insert(make_pair(DI, FI));
				kernels(j, FI, DI, kernelmap);
			}
		}
	}
}

void find_kernels(const polynomial& P, map<monomial, polynomial>& kernelmap)
{
	kernelmap.clear();
	kernels(0, P, monomial(), kernelmap);
	if (P.gcd() == monomial())
	{
		kernelmap.insert(make_pair(monomial(), P));
	}
}
