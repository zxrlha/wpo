#include <set>
#include <map>
#include <boost/lexical_cast.hpp>
#include "output.hpp"
#include "literal.hpp"
#include "yyglobal.hpp"

using std::set;
using std::make_pair;
using std::map;
using std::string;

string output_tmp_name(int i, int rl)
{
    return vtmp_prefix[rl] + boost::lexical_cast<string>(i) + vtmp_suffix[rl];
}

//this function only modify vP, while left vfunc unchanged
//which is a bug, so disabled
/*
void literal_change(vector<polynomial>& vP, int from, int to)
{
    for (auto& P : vP)
    {
        for (int i = 0; i < P.size(); ++i)
        {
            for (int j = 0; j < P[i].size(); ++j)
            {
                if (P[i].lit(j) == from)
                {
                    int p = P[i].pow(j);
                    P[i] *= monomial(to, p);
                    P[i] /= monomial(from, p);
                    break;
                }
            }
        }
    }
}
*/

//this function is correct but literal_change is buggy
/*
void clean()
{
    //find polynomial like A=B
    set<int> remove_list;
    for (int i = 0; i < vP.size(); ++i)
    {
        if (vP[i].size() == 1 && vP[i][0].coef() == 1 && vP[i][0].size() == 1 && vP[i][0].pow(0) == 1)
        {
            int lj = vP[i][0].lit(0);
            if (literal_is_tmp(vP[i].name()))//A=B,A is a termporary name
            {
                remove_list.insert(i);
                literal_change(vP, literal_get(vP[i].name()), lj);//replace all A by B
            }
            else if (literal_is_tmp(literal_name(lj)))//A=B,B is a termporary name
            {
                //find the polynomial for B
                int Bj = -1;
                for (int j = 0; j < vP.size(); ++j)
                {
                    if (vP[j].name() == literal_name(lj))
                    {
                        Bj = j;
                        break;
                    }
                }
                assert(Bj != -1);
                //set this polynomial to A
                vP[Bj].name() = vP[i].name();
                //change all name from B to A
                int Al = literal_get(vP[i].name());
                if (Al == -1)
                {
                    Al = literal_append(vP[i].name());
                }
                literal_change(vP, lj, Al);
                remove_list.insert(i);
            }
        }
    }
    vector<polynomial> vtmp;
    for (int i = 0; i < vP.size(); ++i)
    {
        if (remove_list.count(i) == 0)
        {
            vtmp.push_back(vP[i]);
        }
    }
    vP = std::move(vtmp);
}
*/

void reorder(vector<vector<int>>& vorder)
{
    vorder.clear();
    vorder.resize(literal_maximum_ring_level() + 1);
    //stage 1: build unknown literal
    set<int> unknown_literal;
    //loop over vP and vfunc
    for (const auto& P : vP)
    {
        int i = literal_get(P.name());
        assert(i != -1);
        unknown_literal.insert(i);
    }
    for (const auto& F : vfunc)
    {
        int i = literal_get(F._resname);
        assert(i != -1);
        unknown_literal.insert(i);
    }
    //stage 2: find known literal
    vector<int> un_vf(vfunc.size());//index of all unknown vfunc
    for (int i = 0; i < vfunc.size(); ++i) { un_vf[i] = i; }
    vector<int> un_vp(vP.size());//index of all unknown vP
    for (int i = 0; i < vP.size(); ++i) { un_vp[i] = i; }
    bool flag = true;
    while (flag)
    {
        flag = false;
        for (auto it = un_vf.begin(); it != un_vf.end();)
        {
            if (unknown_literal.count(vfunc[*it]._paraid) == 0)//known!
            {
                flag = true;
                vorder[vfunc[*it].ring_level()].push_back(-1 - (*it));
                //mark as known
                int li = literal_get(vfunc[*it]._resname);
                assert(li != -1);
                unknown_literal.erase(li);
                it = un_vf.erase(it);
            }
            else
            {
                ++it;
            }
        }
        for (auto it = un_vp.begin(); it != un_vp.end();)
        {
            if (!vP[*it].contain_literals(unknown_literal))//known!
            {
                flag = true;
                int li = literal_get(vP[*it].name());
                int lvl = vP[*it].ring_level();
                vorder[lvl].push_back(*it);
                //mark as known
                assert(li != -1);
                unknown_literal.erase(li);
                it = un_vp.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
    assert(un_vf.size() == 0);
    for (int i = 0; i < un_vp.size(); ++i)
    {
        std::cerr<<vP[un_vp[i]]<<std::endl;
    }
    assert(un_vp.size() == 0);
}

//trans_rule: tmp index -> final index in each ring level
string translate_tmp(int id, vector<map<int, int>>& trans_rule, vector<set<int>>& notinuse, vector<int>& vmax, bool newflag, bool removeflag)
{
    int rl = literal_get_ring_level(id);
    auto it = trans_rule[rl].find(id);
    string newname;
    if (it == trans_rule[rl].end())
    {
        //assert(newflag);
        int ni = 0;
        if (notinuse[rl].size() == 0)
        {
            ++vmax[rl];
            ni = vmax[rl];
        }
        else
        {
            auto it = notinuse[rl].begin();
            ni = *it;
            notinuse[rl].erase(it);
        }
        newname = '#' + output_tmp_name(ni, rl);
        trans_rule[rl].insert(make_pair(id, ni));
        if (!newflag) std::cerr<<newname<<std::endl;
    }
    else
    {
        newname = '#' + output_tmp_name(it->second, rl);
    }
    if (removeflag)
    {
        notinuse[rl].insert(trans_rule[rl][id]);
    }
    return newname;
}
void rename(const vector<vector<int>>& vorder, vector<int>& vmax)
{
    vmax.clear();
    for (int i = 0; i < vorder.size(); ++i)
    {
        vmax.push_back(0);
    }
    vector<map<int, int>> trans_rule(vorder.size());
    vector<set<int>> notinuse(vorder.size());
    //rename temporary variables to their approxiate output name,
    //and try to optimize space usage
    for (int rl = 0; rl <= literal_maximum_ring_level(); ++rl)
    {
        //the temporary variables in use for current ring level
        //reverse order
        for (int i = vorder[rl].size() - 1; i >= 0; --i)
        {
            int index = vorder[rl][i];
            if (index < 0)//a func
            {
                index = - 1 - index;
                bool t = literal_is_tmp(vfunc[index]._resname);
                if (t)
                {
                    int ni = literal_get(vfunc[index]._resname);
                    string newname = translate_tmp(ni, trans_rule, notinuse, vmax, false, flag_reuse);
                    vfunc[index]._resname = newname;
                }
                t = literal_is_tmp(vfunc[index]._paraid);
                if (t)
                {
                    translate_tmp(vfunc[index]._paraid, trans_rule, notinuse, vmax, true, false);
                }
            }
            else//a polynomial
            {
                bool t = literal_is_tmp(vP[index].name());
                if (t)
                {
                    int ni = literal_get(vP[index].name());
                    string newname = translate_tmp(ni, trans_rule, notinuse, vmax, false, flag_reuse);
                    vP[index].name() = newname;
                }
                set<int> tsi = vP[index].tmp_literals();
                for (auto ni : tsi)
                {
                    translate_tmp(ni, trans_rule, notinuse, vmax, true, false);
                }
            }
        }
    }
    for (int i = 0; i < trans_rule.size(); ++i)
    {
        for (auto r : trans_rule[i])
        {
            literal_set_name(r.first, output_tmp_name(r.second, i));
        }
    }
}
