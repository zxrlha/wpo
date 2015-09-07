#include <iostream>
#include <set>
#include "kernel.hpp"
#include "extraction.hpp"
#include "kcm.hpp"
#include "literal.hpp"
#include "yyglobal.hpp"
#include "parser.hpp"
#include "output.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::set;
using std::string;

extern int yyparse();

bool pass_filter(const std::string& name)
{
	if (name.find_first_of(var_filter) != std::string::npos)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void debug_out()
{
	for (int i = 0; i < vP.size(); ++i)
	{
		string name = vP[i].name();
		cout << line_prefix;
		cout << vP[i] << line_suffix << endl;
	}
}

void output_func(const string& paraname)
{
	vector<string> vn;
	for (auto it = vfunc.begin(); it != vfunc.end();)
	{
		if (literal_name(it->_paraid) == paraname)
		{
			cout << line_prefix;
			if (func_style == "in")
			{
				cout << type_str << " ";
			}
			cout << it->_resname << "="
			     << it->_funcname << "(" << literal_name(it->_paraid) << ")"
			     << line_suffix << endl;
			vn.push_back(it->_resname);
			it = vfunc.erase(it);
		}
		else
		{
			++it;
		}
	}
	for (auto str : vn)
	{
		output_func(str);
	}
}

void output_prime_func()
{
	set<string> calced;
	for (auto P : vP)
	{
		string ns(P.name());
		if (literal_is_tmp(P.name()))
		{
			ns.erase(ns.begin());
		}
		calced.insert(ns);
	}
	for (auto fe : vfunc)
	{
		calced.insert(fe._resname);
	}
	vector<string> vn;
	for (auto it = vfunc.begin(); it != vfunc.end();)
	{
		if (calced.count(literal_name(it->_paraid)) == 0)
		{
			cout << line_prefix;
			if (func_style == "in")
			{
				cout << type_str << " ";
			}
			cout << it->_resname << "="
			     << it->_funcname << "(" << literal_name(it->_paraid) << ")"
			     << line_suffix << endl;
			vn.push_back(it->_resname);
			it = vfunc.erase(it);
		}
		else
		{
			++it;
		}
	}
	for (auto str : vn)
	{
		output_func(str);
	}
}

int main()
{
	yyparse();
	//cerr << "//parse finished" << endl;
	if (strategy == "sequential")
	{
		vector<polynomial> vtmp(vP);
		vP.clear();
		for (size_t i = 0; i < vtmp.size(); ++i)
		{
			//cerr<<i<<" "<<vtmp.size()<<endl;
			vP.push_back(vtmp[i]);
			find_kernel_intersections(vP);
		}
	}
	else if (strategy == "independent")
	{
		vector<polynomial> vtmp(vP);
		vector<polynomial> vres;
		for (size_t i = 0; i < vtmp.size(); ++i)
		{
			vP.clear();
			vP.push_back(vtmp[i]);
			find_kernel_intersections(vP);
			for (auto j = 0; j < vP.size(); ++j)
			{
				vres.push_back(vP[j]);
			}
		}
		vP = vres;
	}
	else if (strategy == "fastrun")
	{
		fr_find_kernel_intersections(vP);
	}
	else
	{
		find_kernel_intersections(vP);
	}
	//cerr << "//kernel intersection finished" << endl;
	//debug_out();
	find_cube_intersections(vP);
	//cerr << "//cube intersection finished" << endl;
	if (clean)
	{
		doclean(vP);
	}
	reorder(vP);
	//cerr << "//reorder finished" << endl;
	int max = rename(vP);
	//cerr << "//rename finished" << endl;
	if (tmp_style == "array0")
	{
		cout << line_prefix
		     << type_str << " "
		     << output_tmp_name(max + 1)
		     << line_suffix << endl;
	}
	if (tmp_style == "array1")
	{
		cout << line_prefix
		     << type_str << " "
		     << output_tmp_name(max)
		     << line_suffix << endl;
	}
	if (tmp_style == "pre")
	{
		for (int i = 0; i <= max; ++i)
		{
			cout << line_prefix
			     << type_str << " "
			     << output_tmp_name(i)
			     << line_suffix << endl;
		}
	}
	if (var_style == "pre")
	{
		for (int i = 0; i < vP.size(); ++i)
		{
			if (!literal_is_tmp(vP[i].name()) && pass_filter(vP[i].name()))
			{
				cout << line_prefix
				     << type_str << " "
				     << vP[i].name()
				     << line_suffix << endl;
			}
		}
	}
	if (func_style == "pre")
	{
		for (int i = 0; i < vfunc.size(); ++i)
		{
			cout << line_prefix
			     << type_str << " "
			     << vfunc[i]._resname
			     << line_suffix << endl;
		}
	}
	output_prime_func();
	set<string> declaredtmp;
	for (int i = 0; i < vP.size(); ++i)
	{
		string name = vP[i].name();
		cout << line_prefix;
		if (literal_is_tmp(name))
		{
			if (tmp_style == "in" && declaredtmp.count(name) == 0)
			{
				declaredtmp.insert(name);
				cout << type_str << " ";
			}
			vP[i].name().erase(vP[i].name().begin());
		}
		else
		{
			if (var_style == "in" && pass_filter(vP[i].name()))
			{
				cout << type_str << " ";
			}
		}
		cout << vP[i] << line_suffix << endl;
		output_func(vP[i].name());
	}
	return 0;
}
