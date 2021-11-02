#include "yyglobal.hpp"
#include "literal.hpp"
#include <map>
#include <boost/lexical_cast.hpp>

using std::multimap;

multimap<polynomial, int> vPmap;
vector<polynomial> vP;
vector<funcexpr> vfunc;
vector<int> vindex;

vector<string> vtmp_prefix{"tmp"};
vector<string> vtmp_suffix{""};
string num_prefix = "";
string num_suffix = "";
vector<string> vline_prefix{"    "};
vector<string> vline_suffix{";"};
vector<string> vtmp_style{"in"};//pre in null
vector<string> vvar_style{"in"};//pre in null
vector<string> vinfo_prefix{"//"};
vector<string> vinfo_suffix{""};
vector<string> vvar_filter{"[]():."};
vector<string> voutput_filename;
string strategy = "kcm";//kcm fastrun
bool flag_clean = true;
bool flag_reuse = true;
int64_t max_terms = -1;
int64_t threads_num = 1;
string invname = "inv";
string logname = "log";
string expname = "exp";

string in_file = "";

int64_t summul = 0;
int64_t osummul = 0;

//Auxiliary class for lexical_cast
bool to_bool(const std::string& tmpstr)
{
    std::string str(tmpstr);
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    std::istringstream is(str);
    bool b;
    if (str == "0" || str == "false")
    {
        b = false;
    }
    else if (str == "1" || str == "true")
    {
        b = true;
    }
    else
    {
        throw std::invalid_argument("Unknown flag: " + tmpstr);
    }
    return b;
}

void vP_replace(int i, const polynomial& nP)
{
    polynomial& P = vP[i];
    auto er = vPmap.equal_range(P);
    assert(er.first != vPmap.end());
    bool found = false;
    for (auto it = er.first; it != er.second; ++it)
    {
        if (it->second == i)
        {
            vPmap.erase(it);
            vPmap.insert(std::make_pair(nP, i));
            P = nP;
            found = true;
            return;
        }
    }
    assert(found);
}

void vP_rebuild_map()
{
    vPmap.clear();
    for (int i = 0; i < vP.size(); ++i)
    {
        vPmap.insert(std::make_pair(vP[i], i));
    }
}

void vP_push(const polynomial& P)
{
    vPmap.insert(std::make_pair(P, vP.size()));
    vP.push_back(P);
}

int vP_insert(polynomial& P)
{
    auto it = vPmap.find(P);
    if (it != vPmap.end())
    {
        assert(vP[it->second] == P);
        return literal_get(vP[it->second].name());
    }
    else
    {
        int nli = P.single_id();
        if (nli == -1)
        {
            nli = literal_append_tmp();
            P.name() = literal_name(nli);
            literal_set_ring_level(nli, P.ring_level());
            vP_push(std::move(P));
        }
        return nli;
    }
}

int vP_get(const polynomial& P)
{
    auto it = vPmap.find(P);
    if (it != vPmap.end())
    {
        assert(vP[it->second] == P);
        return it->second;
    }
    else
    {
        return -1;
    }
}

int vfunc_insert(funcexpr& fe)
{
    for (int i = 0; i < vfunc.size(); ++i)
    {
        if (fe._funcname == vfunc[i]._funcname
            && fe._paraid == vfunc[i]._paraid)
        {
            return literal_get(vfunc[i]._resname);
        }
    }
    int nfi = literal_append_tmp();
    fe._resname = literal_name(nfi);
    vfunc.push_back(fe);
    literal_set_ring_level(nfi, fe.ring_level());
    return nfi;
}

void parse_options(const string& name, const string& value)
{
    vector<string> vstr_fix{"tmp_prefix", "tmp_suffix", "line_prefix", "line_suffix", "info_prefix", "info_suffix", "tmp_style", "var_style", "var_filter", "output_filename"};
    vector<vector<string>*> vp_fix{&vtmp_prefix, &vtmp_suffix, &vline_prefix, &vline_suffix, &vinfo_prefix, &vinfo_suffix, &vtmp_style, &vvar_style, &vvar_filter, &voutput_filename};
    assert(vstr_fix.size() == vp_fix.size());
    for (int i = 0; i < vstr_fix.size(); ++i)
    {
        if (name.compare(0, vstr_fix[i].size(), vstr_fix[i]) == 0)
        {
            string s_lvl = name.substr(vstr_fix[i].size());
            int lvl;
            if (s_lvl.size() == 0) { lvl = 0; }
            else { lvl = boost::lexical_cast<std::int64_t>(s_lvl); }
            if (vp_fix[i]->size() <= lvl) { vp_fix[i]->resize(lvl + 1); }
            vp_fix[i]->at(lvl) = value;
            return;
        }
    }
    if (name == "num_prefix")
    { num_prefix = value; }
    else if (name == "num_suffix")
    { num_suffix = value; }
    else if (name == "strategy")
    { strategy = value; }
    else if (name == "invname")
    { invname = value; }
    else if (name == "expname")
    { expname = value; }
    else if (name == "logname")
    { logname = value; }
    else if (name == "clean")
    { flag_clean = to_bool(value); }
    else if (name == "reuse")
    { flag_reuse = to_bool(value); }
    else if (name == "max_terms")
    { max_terms = boost::lexical_cast<int>(value); }
    else if (name == "threads_num")
    { threads_num = boost::lexical_cast<int>(value); }
    else if (name.compare(0, 4, "ring") == 0)
    { literal_parse_ring(name.substr(4), value); }
    else
    {
        std::cerr << "ERROR:Unknown option:" << name << std::endl;
        error_end();
    }
}

void init_ring_defaults()
{
    vector<vector<string>*> vp_fix{&vtmp_prefix, &vtmp_suffix, &vline_prefix, &vline_suffix, &vinfo_prefix, &vinfo_suffix, &vtmp_style, &vvar_style, &vvar_filter};
    for (int i = 0; i < vp_fix.size(); ++i)
    {
        if (vp_fix[i]->size() != literal_maximum_ring_level() + 1)
        {
            vp_fix[i]->resize(literal_maximum_ring_level() + 1);
        }
        for (int j = 0; j < vp_fix[i]->size(); ++j)
        {
            if (vp_fix[i]->at(j).size() == 0)
            {
                vp_fix[i]->at(j) = vp_fix[i]->at(0);
            }
        }
    }
}


void error_end()
{
    std::cerr << "Input file: " << in_file << std::endl;
    std::exit(1);
}
