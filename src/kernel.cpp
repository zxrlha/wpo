#include <algorithm>
#include <iostream>
#include <set>
#include <boost/dynamic_bitset.hpp>
#include "kernel.hpp"
#include "literal.hpp"

using std::make_pair;

void kernels(int i, const polynomial& P, const monomial& d, vector<pair<monomial, polynomial>>& kernelmap)
{
    //we use bitset for sj1 as we need fast lookup
    //while for sj2 we need iteration therefore we use set
    std::set<int> sj2;
    //brackets here for early release sj1, to reduce memory cost, as in recursive calling we may waste lots of memory
    {
        boost::dynamic_bitset<uint64_t> sj1(literal_size());//default value should be 0, i.e. false
        for (int mi = 0; mi < P.size(); ++mi)
        {
            for (int ti = 0; ti < P[mi].size(); ++ti)
            {
                int tmp = P[mi].lit(ti);
                if (tmp < i) { continue; }
                if (sj1[tmp] == 0)
                {
                    sj1[tmp] = 1;
                }
                else
                {
                    sj2.insert(tmp);
                }
            }
        }
    }
    for (auto j : sj2)
    {
        monomial Lj(j);
        polynomial Ft = P / Lj;
        monomial C = Ft.gcd();
        //optimization for this common case
        if (C == monomial())
        {
            monomial DI = d * Lj;//co-kernel
            kernelmap.push_back(make_pair(DI, Ft));
            kernels(j, Ft, DI, kernelmap);
            continue;
        }
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
