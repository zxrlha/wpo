#include <cstdlib>
#include <vector>
#include <map>
#include <boost/lexical_cast.hpp>
#include "literal.hpp"
#include "yyglobal.hpp"

using std::vector;
using std::map;

namespace
{
	vector<string> vlit;
	vector<int> vlitlvl;
	int tmpi = 0;
	int funci = 0;
	vector<string> vrtype;
	map<string, int> vidlvl;
}

int literal_append(const string& name)
{
	vlit.push_back(name);
	int lvl = literal_find_ring_level(name);
	vlitlvl.push_back(lvl);
	return vlit.size() - 1;
}
int literal_find_ring_level(const string& name)
{
	int lvl;
	auto it = vidlvl.find(name);
	if (it != vidlvl.end())
	{
		lvl = it->second;
	}
	else
	{
		lvl = 0;
	}
	return lvl;
}

int literal_append_tmp()
{
	++tmpi;
	return literal_append("#tmp" + boost::lexical_cast<string>(tmpi));
}

int literal_append_func()
{
	++funci;
	return literal_append(func_prefix + boost::lexical_cast<string>(funci));
}

bool literal_is_tmp(const string& name)
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

bool literal_is_tmp(int i)
{
	return literal_is_tmp(vlit[i]);
}


string literal_name(int i)
{
	return vlit[i];
}

int literal_size()
{
	return vlit.size();
}

bool literal_check(const string& name)
{
	for (auto t : vlit)
	{
		if (t == name)
		{
			return true;
		}
	}
	return false;
}

int literal_get(const string& name)
{
	for (int i = 0; i < vlit.size(); ++i)
	{
		if (vlit[i] == name)
		{
			return i;
		}
	}
	return -1;
}


int literal_add(const string& name, bool numflag)
{
	for (int i = 0; i < vlit.size(); ++i)
	{
		if (vlit[i] == name)
		{
			return i;
		}
	}
	int i = literal_append(name);
	if (numflag)
	{
		literal_set_ring_level(i, literal_maxium_ring_level());//set maxium level for numbers
	}
	return i;
}

void literal_set_name(int i, const string& name)
{
	vlit[i] = name;
}

void literal_set_ring_level(int i, int lvl)
{
	vlitlvl[i] = lvl;
}

int literal_get_ring_level(int i)
{
	return vlitlvl[i];
}

int literal_maxium_ring_level()
{
	return vrtype.size() - 1;
}

string literal_get_ring_type(int i)
{
	return vrtype[i];
}

void literal_parse_ring(const string& name, const string& value)
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
		vidlvl[value] = level;
	}
	else
	{
		std::cerr << "ERROR:" << "Unknown ring options: " << name << std::endl;
		error_end();
	}
}

bool literal_ring_type_check()
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
