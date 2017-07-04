#include <map>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <functional>
#include <thread>
#include "kcm.hpp"
#include "extraction.hpp"
#include "kernel.hpp"
#include "literal.hpp"
#include "yyglobal.hpp"

using std::map;

void kcm_find_kernel_intersections(vector<polynomial>& vP)
{
    int sumbv1 = 0;
    int bv = 0;
    while (true)
    {
        vector<pair<monomial, polynomial>> vmp;
        vector<vector<pair<monomial, polynomial>>> vkmap;
        for (auto P : vP)
        {
            find_kernels(P, vmp);
            vkmap.push_back(std::move(vmp));
        }
        kcm tm(vkmap, vP);
        vkmap.clear();
        vector<int> vr;
        vector<int> vc;
        bv = tm.generate_best_rectangle(vr, vc);
        if (bv < 0)
        {
            break;
        }
        else
        {
            //build new literal polynomial
            polynomial nlp;
            int li;
            li = literal_append_tmp();
            for (auto vci : vc)
            {
                nlp += tm.column(vci);
            }
            bv = 0;
            for (auto vri : vr) //loop over co-kernel
            {
                int pi = tm.index(vri);//P index
                polynomial& P = vP[pi];
                bool flag = true;
                //check
                for (auto vci : vc)
                {
                    if (!P.contain(tm.row(vri)*tm.column(vci)))
                    {
                        flag = false;
                        break;
                    }
                }
                if (!flag)
                {
                    continue;
                }
                //rewrote P
                polynomial nP;
                int64_t before = P.multiplication_number();
                for (auto vci : vc)
                {
                    P.remove(tm.row(vri)*tm.column(vci));
                }
                monomial nm(li);
                P += nm * tm.row(vri);
                int64_t after = P.multiplication_number();
                bv += before - after;
            }
            vP.push_back(nlp);
            vP.back().name() = literal_name(li);
            literal_set_ring_level(li, nlp.ring_level());
            bv -= nlp.multiplication_number();
            sumbv1 += bv;
            summul -= bv;
            std::cerr << "Step 1: Descrease by" << std::setw(7) << bv << "."
                      << "Total descreased:" << std::setw(8) << sumbv1 << "."
                      << "Remain" << std::setw(8) << summul
                      << "(" << std::setw(5) << double(summul) / osummul * 100 << "%)" << "\r";
        }
    }
}

bool fr_ring_factorization(const polynomial& P, monomial& m)
{
    int rl = P.ring_level();
    int v = 0;
    int b1 = -1;
    for (int i = 0; i < P.size(); ++i)
    {
        int ps = P[i].ring_pow_sum(rl, rl);
        if (ps > v)
        {
            int mp = P[i].multiplication_number();
            if (mp - ps >= 2)//valid
            {
                b1 = i;
                v = ps;
            }
        }
    }
    if (b1 != -1)
    {
        m = P[b1].ring_sub_mon(rl, rl);
        return true;
    }
    return false;
}

bool fr_kernel_intersection(const polynomial& P, monomial& m)
{
    int minlvl = P.ring_level();
    int maxlvl = minlvl;
    int v = 0;
    int bi = -1;
    int bj = -1;
    for (int i = 0; i < P.size(); ++i)
    {
        for (int j = i + 1; j < P.size(); ++j)
        {
            int mn = gcd_mn(P[i], P[j], minlvl, maxlvl);
            if (mn > v)
            {
                bi = i;
                bj = j;
                v = mn;
            }
        }
    }
    if (bi == -1)
    {
        return false;
    }
    m = gcd(P[bi], P[bj], minlvl, maxlvl);
    return true;
}

bool fr_ring_kernel_intersection(const polynomial& P, monomial& m)
{
    //first perform factorization
    if (fr_ring_factorization(P, m)) return true;
    //then kernel intersection
    if (fr_kernel_intersection(P, m)) return true;
    return false;
}

void fr_find_kernel_intersections(vector<polynomial>& vP)
{
    int sumbv1 = 0;
    int pi = 0;
    while (true)
    {
        monomial bk;
        bool flag = false;
        for (; pi < vP.size(); ++pi)
        {
            flag = fr_ring_kernel_intersection(vP[pi], bk);
            if (flag)
            {
                break;
            }
        }
        if (!flag)
        {
            break;
        }
        //build new literal polynomial
        polynomial nP = vP[pi];
        polynomial bcok = nP / bk;
        int64_t before = nP.multiplication_number();
        for (int i = 0; i < bcok.size(); ++i)
        {
            nP.remove(bk * bcok[i]);
        }
        int li;
        li = vP_get(bcok);
        bool newflag = false;
        if (li == -1)
        {
            newflag = true;
            li = literal_append_tmp();
        }
        else
        {
            li = literal_get(vP[li].name());
        }
        bcok.name() = literal_name(li);
        monomial nm(li);
        nP += nm * bk;
        //now we should modify both P in vP and vPmap
        vP_replace(pi, nP);
        int64_t after = nP.multiplication_number();
        if (newflag)
        {
            vP_push(bcok);
            literal_set_ring_level(li, bcok.ring_level());
        }
        if (newflag)
        {
            after += bcok.multiplication_number();
        }
        int bv = before - after;
        sumbv1 += bv;
        summul -= bv;
        std::cerr << "Step 1: Descrease by" << std::setw(7) << bv << "."
                  << "Total descreased:" << std::setw(8) << sumbv1 << "."
                  << "Remain" << std::setw(8) << summul
                  << "(" << std::setw(5) << double(summul) / osummul * 100 << "%)" << "\r";
    }
}

void find_kernel_intersections(vector<polynomial>& vP)
{
    if (strategy == "kcm")
    {
        kcm_find_kernel_intersections(vP);
    }
    else if (strategy == "fastrun")
    {
        fr_find_kernel_intersections(vP);
    }
    else
    {
        std::cerr << "ERROR: unknown strategy " << strategy << std::endl;
        error_end();
    }
    std::cerr << std::endl;
}

bool fr_parts_cube_intersection(const vector<monomial>& mat, monomial& m, int minlvl, int maxlvl)
{
    int v = 1;
    int bi = -1;
    int bj = -1;
    for (int i = 0; i < mat.size(); ++i)
    {
        for (int j = i + 1; j < mat.size(); ++j)
        {
            int mn = gcd_mn(mat[i], mat[j], minlvl, maxlvl);
            if (mn > v)
            {
                bi = i;
                bj = j;
                v = mn;
            }
        }
    }
    if (bi == -1)
    {
        return false;
    }
    m = gcd(mat[bi], mat[bj], minlvl, maxlvl);
    return true;
}

bool fr_cube_intersection(const vector<polynomial>& vP, monomial& m, int minlvl, int maxlvl)
{
    if (max_terms == 0)
    {
        std::vector<monomial> mat;
        for (int i = 0; i < vP.size(); ++i)
        {
            for (int j = 0; j < vP[i].size(); ++j)
            {
                mat.push_back(vP[i][j]);
            }
        }
        return fr_parts_cube_intersection(mat, m, minlvl, maxlvl);
    }
    else
    {
        //Inspired by Shellsort
        //We record previous runs to speed up
        //This is the steps size
        static int64_t steps = 0;
        static int64_t base_shift = 0;
        static int64_t start_index = 0;
        //Initialize step size according to max_terms
        //the predefined setting for max_terms
        if (max_terms == -1) { max_terms = sqrt(vP.size()); }
        if (steps == 0) { steps = vP.size() / max_terms + 1; }
        //now start
        for (; steps != 0; --steps)
        {
            for (; base_shift < vP.size(); base_shift += steps * max_terms)
            {
                for (; start_index < steps; ++start_index)
                {
                    //set the matrix
                    std::vector<monomial> mat;
                    for (int cti = 0; cti < max_terms; ++cti)
                    {
                        int index = start_index + cti * steps + base_shift;
                        if (index >= vP.size()) { break; }
                        for (int j = 0; j < vP[index].size(); ++j)
                        {
                            mat.push_back(vP[index][j]);
                        }
                    }
                    int status = fr_parts_cube_intersection(mat, m, minlvl, maxlvl);
                    if (status) { return true; }
                }
                //reset start_index
                start_index = 0;
            }
            //reset base_shift
            base_shift = 0;
        }
        return false;
    }
}

void find_cube_intersections(vector<polynomial>& vP)
{
    int sumbv2 = 0;
    //for cube part, maxlvl is always maximum ring level,
    //and we decrease minlvl till 0
    int minlvl = literal_maximum_ring_level();
    int maxlvl = literal_maximum_ring_level();
    while (true)
    {
        monomial m;
        if (!fr_cube_intersection(vP, m, minlvl, maxlvl))
        {
            if (minlvl == 0) { break; }
            else
            {
                --minlvl;
                continue;
            }
        }
        int li = literal_append_tmp();
        //rewrote vP
        int rs = 0;
        for (auto& P : vP)
        {
            polynomial dres = P / m;
            if (dres.size() == 0)
            {
                continue;
            }
            ++rs;
            polynomial nP(P);
            for (int i = 0; i < dres.size(); ++i)
            {
                nP.remove(dres[i]*m);
            }
            nP.name() = P.name();
            monomial nl(li);
            for (int i = 0; i < dres.size(); ++i)
            {
                nP += dres[i] * nl;
            }
            P = nP;
        }
        int64_t bv = (rs - 1) * (m.multiplication_number() - 1);
        sumbv2 += bv;
        summul -= bv;
        std::cerr << "Step 2: Descrease by" << std::setw(7) << bv << "."
                  << "Total descreased:" << std::setw(8) << sumbv2 << "."
                  << "Remain" << std::setw(8) << summul
                  << "(" << std::setw(5) << double(summul) / osummul * 100 << "%)" << "\r";
        //add this into vP
        polynomial nl;
        nl += m;
        nl.name() = literal_name(li);
        vP.push_back(nl);
        literal_set_ring_level(li, nl.ring_level());
    }
    std::cerr << std::endl;
}
