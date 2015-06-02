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
	return tmp_prefix + boost::lexical_cast<string>(i + tmp_start) + tmp_suffix;
}

void update_vf_unknown_literal(int ni, set<int>& unknown_literal)
{
	vector<int> vk;
	//now also update known vfunc
	for (int fi = 0; fi < vfunc.size(); ++fi)
	{
		if (vfunc[fi]._paraid == ni)
		{
			int ki = literal_get(vfunc[fi]._resname);
			unknown_literal.erase(ki);
			vk.push_back(ki);
		}
	}
	for (auto ti : vk)
	{
		update_vf_unknown_literal(ti, unknown_literal);
	}
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
		bool flag = true;
		vector<funcexpr> tvf(vfunc);
		while (flag)
		{
			flag = false;
			for (auto it = tvf.begin(); it != tvf.end();)
			{
				if (unknown_literal.count(it->_paraid) != 0)//unknown func
				{
					flag = true;
					unknown_literal.insert(literal_get(it->_resname));
					it = tvf.erase(it);
				}
				else
				{
					++it;
				}
			}
		}
		vector<polynomial> rvP;
		while (vP.size() != 0)
		{
			for (int i = 0; i != vP.size();)
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
					vP.erase(vP.begin() + i);
					update_vf_unknown_literal(li, unknown_literal);
				}
			}
		}
		vP = rvP;
	}


	int rename(vector<polynomial>& rvP)
	{
		int max = -1;
		//now rename to optimization space usage
		vector<polynomial> ornv(rvP.rbegin(), rvP.rend());
		map<int, int> trans_rule;
		set<int> inuse;//in use
		for (auto& P : ornv)
		{
			bool t = literal_is_tmp(P.name());
			if (t)
			{
				int i = literal_get(P.name());
				auto it = trans_rule.find(i);
				if (it == trans_rule.end())//new tmp_literal
				{
					int ni = 0;
					while (inuse.count(ni) != 0) ++ni;
					trans_rule.insert(make_pair(i, ni));
					inuse.insert(ni);
					if (ni > max) max = ni;
				}
				int pi = trans_rule[i];
				inuse.erase(pi);
				P.name() = "#" + output_tmp_name(pi);//mark as tmp variable
			}
			set<int> tsi = P.tmp_literals();
			for (auto i : tsi)
			{
				auto it = trans_rule.find(i);
				if (it == trans_rule.end())//new tmp_literal
				{
					int ni = 0;
					while (inuse.count(ni) != 0) ++ni;
					trans_rule.insert(make_pair(i, ni));
					inuse.insert(ni);
					if (ni > max) max = ni;
				}
			}
		}
		for (auto r : trans_rule)
		{
			literal_set_name(r.first, output_tmp_name(r.second));
		}

		std::copy(ornv.rbegin(), ornv.rend(), rvP.begin());
		return max;
	}
