#ifndef WPO_LITERAL_HPP
#define WPO_LITERAL_HPP 1

#include <string>
#include <cstdlib>
#include <vector>
#include <map>
#include <regex>
#include <boost/lexical_cast.hpp>
#include "yyglobal.hpp"

using std::string;

//append a new literal, its ring level will be set into 0, unless it is specifical defined in ringid
int literal_append(const string& name);
//append a new tmp literal, its ring level will be set into 0
int literal_append_tmp();
//get the name of this literal
string literal_name(int i);
//return the number of literals
int literal_size();

void literal_set_name(int i, const string& name);

bool literal_check(const string& name);
int literal_add(const string& name, bool num_flag = false);
int literal_get(const string& name);
bool literal_is_tmp(const string& name);
bool literal_is_tmp(int i);

void literal_set_ring_level(int i, int lvl);
int literal_get_ring_level(int i);
int literal_maximum_ring_level();
string literal_get_ring_type(int i);
int literal_get_default_ring_level();
void literal_set_default_ring_level(int lvl);
int literal_find_ring_level(const string& name);

void literal_parse_ring(const string& name, const string& value);

bool literal_ring_type_check();

using std::vector;
using std::map;

extern vector<string> vlit;
extern map<string, int> vlit_map;
extern vector<int> vlitlvl;
extern int tmpi;
extern vector<string> vrtype;
extern vector<std::pair<std::regex, int>> vidlvl;
extern int dftlvl;

inline int literal_append(const string& name)
{
    vlit.push_back(name);
    int lvl = literal_find_ring_level(name);
    if (lvl == -1) lvl = literal_get_default_ring_level();
    vlitlvl.push_back(lvl);
    vlit_map.insert(std::make_pair(name, vlit.size() - 1));
    return vlit.size() - 1;
}

inline int literal_append_tmp()
{
    ++tmpi;
    return literal_append("#tmp" + boost::lexical_cast<string>(tmpi));
}

inline bool literal_is_tmp(const string& name)
{
    if (name[0] == '#')
    {
        return true;
    }
    else
    {
        return false;
    }
}

inline bool literal_is_tmp(int i)
{
    if (i > vlit.size()) { std::cerr << i << std::endl; }
    assert(i < vlit.size());
    return literal_is_tmp(vlit[i]);
}


inline string literal_name(int i)
{
    return vlit[i];
}

inline int literal_size()
{
    return vlit.size();
}

inline bool literal_check(const string& name)
{
    auto it = vlit_map.find(name);
    if (it != vlit_map.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

inline int literal_get(const string& name)
{
    auto it = vlit_map.find(name);
    int res = 0;
    if (it == vlit_map.end())
    {
        return -1;
    }
    else
    {
        return it->second;
    }
}


inline int literal_add(const string& name, bool numflag)
{
    string newname = name;
    if (numflag)//we need to add prefix and suffix
    {
        newname = num_prefix + name + num_suffix;
    }
    int i = literal_get(newname);
    if (i != -1) { return i; }
    i = literal_append(newname);
    if (numflag)
    {
        literal_set_ring_level(i, literal_maximum_ring_level());//set maximum level for numbers
    }
    return i;
}

inline void literal_set_name(int i, const string& name)
{
    auto it = vlit_map.find(vlit[i]);
    //this function is called when output code, so we shouldn't change vlit_map, because different literals may have same name(if reuse is on)
    //vlit_map.erase(it);
    vlit[i] = name;
    //vlit_map.insert(std::make_pair(name, i));
}

inline void literal_set_ring_level(int i, int lvl)
{
    vlitlvl[i] = lvl;
}

inline int literal_get_ring_level(int i)
{
    return vlitlvl[i];
}

inline int literal_maximum_ring_level()
{
    return vrtype.size() - 1;
}

inline int literal_get_default_ring_level()
{
    return dftlvl;
}

inline void literal_set_default_ring_level(int lvl)
{
    dftlvl = lvl;
}

inline string literal_get_ring_type(int i)
{
    return vrtype[i];
}

inline void literal_parse_ring(const string& name, const string& value)
{
    if (name.compare(0, 4, "type") == 0)
    {
        int level = boost::lexical_cast<std::int64_t>(name.substr(4));
        if (level + 1 > vrtype.size())
        {
            vrtype.resize(level + 1);
        }
        vrtype[level] = value;
    }
    else if (name.compare(0, 2, "id") == 0)
    {
        int level = boost::lexical_cast<std::int64_t>(name.substr(2));
        vidlvl.push_back({std::regex(value), level});
    }
    else if (name == "_default_level")
    {
        int level = boost::lexical_cast<std::int64_t>(value);
        dftlvl = level;
    }
    else
    {
        std::cerr << "ERROR:" << "Unknown ring options: " << name << std::endl;
        error_end();
    }
}

inline bool literal_ring_type_check()
{
    for (int i = 0; i < vrtype.size(); ++i)
    {
        if (vrtype[i] == "")
        {
            return false;
        }
    }
    return true;
}

#endif
