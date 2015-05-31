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

string output_tmp_name(int i)
{
	return tmp_prefix+boost::lexical_cast<string>(i+tmp_start)+tmp_suffix;
}

void reorder(vector<polynomial>& vP)
{
	set<int> unknown_literal;
	for (const auto& P : vP)
	{
		int i = literal_get(P.name());
		if (i != -1)
		{
			unknown_literal.insert(i);
		}
	}
	vector<polynomial> rvP;
	while (vP.size() != 0)
	{
		for (int i = 0; i != vP.size(); )
		{
			if (vP[i].contain_literals(unknown_literal))
			{
				++i;
				continue;
			}
			else
			{
				rvP.push_back(vP[i]);
				//mark as known
				int li = literal_get(vP[i].name());
				if (li != -1)
				{
					unknown_literal.erase(li);
				}
				vP.erase(vP.begin()+i);
			}
		}
	}
	vP = rvP;
}


void rename(vector<polynomial>& rvP)
{
	//now rename to optimization space usage
	vector<polynomial> ornv(rvP.rbegin(), rvP.rend());
	map<int, int> trans_rule;
	set<int> inuse;//in use
	for (auto& P: ornv)
	{
		bool t = literal_is_tmp(P.name());
		if (t)
		{
			int pi = trans_rule[literal_get(P.name())];
			inuse.erase(pi);
			P.name() = output_tmp_name(pi);
		}
		set<int> tsi = P.tmp_literals();
		for (auto i: tsi)
		{
			auto it = trans_rule.find(i);
			if (it == trans_rule.end())//new tmp_literal
			{
				int ni = 0;
				while (inuse.count(ni) != 0) ++ni;
				trans_rule.insert(make_pair(i, ni));
				inuse.insert(ni);
			}
		}
	}
	for (auto r: trans_rule)
	{
		literal_set_name(r.first, output_tmp_name(r.second));
	}

	std::copy(ornv.rbegin(), ornv.rend(), rvP.begin());
}
