#include <vector>
#include <boost/lexical_cast.hpp>
#include "literal.hpp"

using std::vector;

namespace
{
	vector<string> vlit;
	int tmpi=0;
}

int literal_append(const string& name)
{
	vlit.push_back(name);
	return vlit.size() - 1;
}

int literal_append_tmp()
{
	++tmpi;
	return literal_append("tmp"+boost::lexical_cast<string>(tmpi));
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
			return true;
	}
	return false;
}

int literal_add(const string& name)
{
	for (int i = 0; i < vlit.size(); ++i)
	{
		if (vlit[i] == name)
		{
			return i;
		}
	}
	return literal_append(name);
}
