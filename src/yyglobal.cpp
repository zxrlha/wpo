#include "yyglobal.hpp"

vector<polynomial> vP;

string tmp_prefix = "tmp[";
string tmp_suffix = "]";
int tmp_start = 0;
string line_suffix = ";";

int vP_get(const polynomial& P)
{
	for (int i = 0; i < vP.size(); ++i)
	{
		if (nosort_equal(P, vP[i])) return i;
	}
	return -1;
}
