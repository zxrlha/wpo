#include <map>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <functional>
#include <thread>
#include <boost/dynamic_bitset.hpp>
#include "kcm.hpp"
#include "extraction.hpp"
#include "kernel.hpp"
#include "literal.hpp"
#include "yyglobal.hpp"

using std::map;

void kcm_find_kernel_intersections(vector<polynomial>& vP, int minlvl, int maxlvl)
{
    int sumbv1 = 0;
    int bv = 0;
    while (true)
    {
        vector<pair<monomial, polynomial>> vmp;
        vector<vector<pair<monomial, polynomial>>> vkmap;
        vector<int> vtrans;//index trans, since now index in vkmap is no longer index in vP
        for (int i = 0; i < vP.size(); ++i)
        {
            int lvl = vP[i].ring_level();
            if (lvl < minlvl || lvl > maxlvl) { continue; }
            find_kernels(vP[i], vmp);
            vkmap.push_back(std::move(vmp));
            vtrans.push_back(i);
        }
        kcm tm(std::move(vkmap));
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
            for (auto vci : vc)
            {
                nlp += tm.column(vci);
            }
            int bpi;
            bpi = vP_get(nlp);
            bool newflag = false;
            int li;
            if (bpi == -1)
            {
                newflag = true;
                li = literal_append_tmp();
            }
            else
            {
                li = literal_get(vP[bpi].name());
            }
            bv = 0;
            bool rewrote = false;//indicate this (co-)kernel is really used
            for (auto vri : vr) //loop over co-kernel
            {
                int pi = vtrans[tm.index(vri)];//P index
                if (pi == bpi) { continue; }//NOTE that one polynomial itself may be the co-kernel, in this case we should skip
                polynomial nP = vP[pi];
                bool flag = true;
                //check
                for (auto vci : vc)
                {
                    if (!nP.contain(tm.row(vri)*tm.column(vci)))
                    {
                        flag = false;
                        break;
                    }
                }
                if (!flag)
                {
                    continue;
                }
                rewrote = true;
                //rewrote P
                int64_t before = nP.multiplication_number();
                for (auto vci : vc)
                {
                    nP.remove(tm.row(vri)*tm.column(vci));
                }
                monomial nm(li);
                nP += nm * tm.row(vri);
                vP_replace(pi, nP);
                int64_t after = nP.multiplication_number();
                bv += before - after;
            }
            nlp.name() = literal_name(li);
            if (newflag)
            {
                literal_set_ring_level(li, nlp.ring_level());
                if (rewrote) //only push this if it is really used
                {
                    vP_push(nlp);
                }
            }
            if (newflag)
            {
                bv -= nlp.multiplication_number();
            }
            sumbv1 += bv;
            summul -= bv;
            std::cerr << "Step 1: Descrease by" << std::setw(7) << bv << "."
                      << "Total descreased:" << std::setw(8) << sumbv1 << "."
                      << "Remain" << std::setw(8) << summul
                      << "(" << std::setw(5) << double(summul) / osummul * 100 << "%)" << "\r";
        }
    }
    std::cerr << std::endl;
}
bool fr_ring_factorization(const polynomial& P, monomial& m, polynomial& bcok)
{
    int rl = literal_get_ring_level(literal_get(P.name()));
    if (rl == literal_maximum_ring_level()) { return false; }
    //first factorize a+b
    bcok = polynomial();
    m = monomial();
    for (int i = 0; i < P.size(); ++i)
    {
        int ps = P[i].ring_pow_sum(rl, rl);
        if (ps == 0)
        {
            bcok += P[i];
        }
    }
    if (bcok.size() >= 2 || (bcok.size() == 1 && bcok[0].multiplication_number() >= 2))
    {
        return true;
    }
    //then factorize high level coefficients
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
        bcok = P / m;
        return true;
    }
    return false;
}
void fr_find_ring_factorization(vector<polynomial>& vP)
{
    int sumbv1 = 0;
    int pi = 0;
    while (true)
    {
        monomial bk;
        polynomial bcok;
        bool flag = false;
        for (; pi < vP.size(); ++pi)
        {
            flag = fr_ring_factorization(vP[pi], bk, bcok);
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
        int64_t before = nP.multiplication_number();
        for (int i = 0; i < bcok.size(); ++i)
        {
            nP.remove(bk * bcok[i]);
        }
        int li;
        li = vP_get(bcok);
        if (li == pi)//identical to the current one, ignore it
        {
            li = -1;
        }
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
        std::cerr << "Step " << 0 << ": Descrease by" << std::setw(7) << bv << "."
                  << "Total descreased:" << std::setw(8) << sumbv1 << "."
                  << "Remain" << std::setw(8) << summul
                  << "(" << std::setw(5) << double(summul) / osummul * 100 << "%)" << "\r";
    }
    std::cerr << std::endl;
}
bool fr_kernel_intersection(const polynomial& P, monomial& m)
{
    int minlvl = literal_get_ring_level(literal_get(P.name()));
    int maxlvl = minlvl;
    //we only need kernels at this ring_level
    //any terms in higher ring level should already been factorized out,
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
            flag = fr_kernel_intersection(vP[pi], bk);
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
        std::cerr << "Step " << 1 << ": Descrease by" << std::setw(7) << bv << "."
                  << "Total descreased:" << std::setw(8) << sumbv1 << "."
                  << "Remain" << std::setw(8) << summul
                  << "(" << std::setw(5) << double(summul) / osummul * 100 << "%)" << "\r";
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
            //std::cout << steps << std::endl;
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
                            //exclude meaningless terms
                            if (vP[index][j].multiplication_number() >= 2)
                            { mat.push_back(vP[index][j]); }
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
    //manage a database for literals in each polynomial for faster lookup
    std::vector<std::vector<int>> vdb;
    vdb.reserve(2 * vP.size());
    auto build_vdb_term = [&vdb, &vP](int i)
    {
        if (i >= vdb.size()) { vdb.resize(i + 1); }
        vdb[i].clear();
        for (int j = 0; j < vP[i].size(); ++j)
        {
            for (int k = 0; k < vP[i][j].size(); ++k)
            {
                vdb[i].insert(std::upper_bound(vdb[i].begin(), vdb[i].end(), vP[i][j].lit(k)), vP[i][j].lit(k));
            }
        }
    };
    auto not_in_vdb = [&vdb](int i, monomial m)
    {
        for (int j = 0; j < m.size(); ++j)
        {
            if (!std::binary_search(vdb[i].begin(), vdb[i].end(), m.lit(j))) { return true; }
        }
        return false;
    };
    //initialize vdb
    for (int i = 0; i < vP.size(); ++i)
    {
        build_vdb_term(i);
    }
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
        //preparing to add this to vP
        polynomial npoly;
        npoly += m;
        int ni;
        ni = vP_get(npoly);
        int li;
        bool newflag = false;
        if (ni == -1)
        {
            newflag = true;
            li = literal_append_tmp();
        }
        else
        {
            li = literal_get(vP[ni].name());
        }
        //rewrote vP except the one which is just "P=m"
        int rs = 0;
        //std::cerr << m << std::endl;
        for (int pi = 0; pi < vP.size(); ++pi)
        {
            const auto& P = vP[pi];
            if (pi == ni) { continue; }
            //check
            if (not_in_vdb(pi, m)) { continue; }
            polynomial dres = P / m;
            rs += dres.size();
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
            vP_replace(pi, nP);
            build_vdb_term(pi);//update vdb
        }
        int64_t bv = (rs - 1) * (m.multiplication_number() - 1);
        sumbv2 += bv;
        summul -= bv;
        std::cerr << "Step 2: Descrease by" << std::setw(7) << bv << "."
                  << "Total descreased:" << std::setw(8) << sumbv2 << "."
                  << "Remain" << std::setw(8) << summul
                  << "(" << std::setw(5) << double(summul) / osummul * 100 << "%)" << "\r";
        //add this into vP
        npoly.name() = literal_name(li);
        if (newflag)
        {
            vP_push(npoly);
            literal_set_ring_level(li, npoly.ring_level());
            build_vdb_term(vP.size() - 1);
        }
    }
    std::cerr << std::endl;
}
void find_intersections(vector<polynomial>& vP)
{
    if (strategy == "kcm")
    {
        kcm_find_kernel_intersections(vP, 0, literal_maximum_ring_level());
        find_cube_intersections(vP);
    }
    else if (strategy == "fastrun")
    {
        fr_find_ring_factorization(vP);//ring factorization
        fr_find_kernel_intersections(vP);//previous kernel intersection
        find_cube_intersections(vP);//cube factorization
    }
    else if (strategy == "kcm0fr")
    {
        fr_find_ring_factorization(vP);//ring factorization
        kcm_find_kernel_intersections(vP, 0, 0);
        //fr_find_kernel_intersections(vP);//previous kernel intersection,useless since we have kcm
        //find_cube_intersections(vP);//cube factorization
    }
    else if (strategy == "none")
    {
    }
    else
    {
        std::cerr << "ERROR: unknown strategy " << strategy << std::endl;
        error_end();
    }
}
