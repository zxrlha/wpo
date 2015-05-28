#include "kernel.hpp"

void kernels(int i, const polynomial& P, const monomial& d, set<monomial>& sco_kernels, set<polynomial>& skernels)
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
				sco_kernels.insert(DI);
				skernels.insert(FI);
				kernels(j, FI, DI, sco_kernels, skernels);
			}
		}
	}
}

void find_kernels(const vector<polynomial>& vP, set<monomial>& sco_kernels, set<polynomial>& skernels)
{
	sco_kernels.clear();
	skernels.clear();
	for (auto P: vP)
	{
		kernels(0, P, monomial(), sco_kernels, skernels);
		if (P.gcd() == monomial())
		{
			sco_kernels.insert(monomial());
			skernels.insert(P);
		}
	}
}
