#ifndef WPO_CIM_HPP
#define WPO_CIM_HPP 1

#include <set>
#include <list>
#include "expression.hpp"

using std::set;
using std::list;
using std::pair;

class llmonomial
{
public:
	llmonomial() = default;
	llmonomial(const llmonomial&) = default;
	llmonomial(llmonomial&&) = default;
	llmonomial(const monomial& m)
	{
		for (int i = 0; i < m.size(); ++i)
		{
			if (m[i] != 0) _vpow.push_back(std::make_pair(i, m[i]));
		}
	};

	int multiplication_number() const
	{
		int res = 0;
		for (auto p : _vpow)
		{
			res += p.second;
		}
		return res;
	}

	list<pair<int, int>> _vpow;
};


class cim
{
public:
	explicit cim(const vector<polynomial>& vP);
	bool generate_best_rectangle(vector<int>& row, monomial& m);
protected:
	void generate_best_rectangle(int ri);
	void generate_best_rectangle(vector<int>& row, const llmonomial& m, set<int> posi);
	int value_of_prime_rectangle(vector<int>& row, const llmonomial& m);
	vector<monomial> _mat;

	vector<int> _br;
	monomial _bm;
	int _bv;
};

#endif
