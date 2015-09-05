#include "yyglobal.hpp"

vector<polynomial> vP;
vector<funcexpr> vfunc;

string tmp_prefix = "tmp[";
string tmp_suffix = "]";
int tmp_start = 0;
string line_suffix = ";";
string line_prefix = "    ";
string type_str = "double";
string tmp_style = "array0";//array0 array1 pre in null
string var_style = "in"; //pre in null
string var_filter = "[]():.";
string func_prefix = "func";
string func_style = "in";
bool sequential = false;
bool clean = true;

int vP_get(const polynomial& P)
{
	for (int i = 0; i < vP.size(); ++i)
	{
		if (nosort_equal(P, vP[i])) return i;
	}
	return -1;
}

int vfunc_get(const funcexpr& fe)
{
	for (int i = 0; i < vfunc.size(); ++i)
	{
		if (fe._funcname == vfunc[i]._funcname
		        && fe._paraid == vfunc[i]._paraid)
			return i;
	}
	return -1;
}
void parse_options(const string& name, const string& value)
{
	if (name == "tmp_prefix") tmp_prefix = value;
	else if (name == "tmp_suffix") tmp_suffix = value;
	else if (name == "line_prefix") line_prefix = value;
	else if (name == "line_suffix") line_suffix = value;
	else if (name == "type") type_str = value;
	else if (name == "tmp_style") tmp_style = value;
	else if (name == "var_style") var_style = value;
	else if (name == "var_filter") var_filter = value;
	else if (name == "func_prefix") func_prefix = value;
	else if (name == "func_style") func_style = value;
	else if (name == "sequential") sequential = (value == "true");
	else if (name == "clean") clean = (value == "true");
	else
	{
		std::cerr << "ERROR:Unknown option:" << name << std::endl;
		exit(1);
	}
}
