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

int main()
{
	yyparse();
	cerr << "//Parse finished" << endl;
	find_kernel_intersections(vP);
	cerr << "//kernel intersection finished" << endl;
	find_cube_intersections(vP);
	cerr << "//cube intersection finished" << endl;

	reorder(vP);
	cerr << "//reorder finished" << endl;
	int max = rename(vP);
	cerr << "//rename finished" << endl;

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
			if (!literal_is_tmp(vP[i].name()))
			{
				cout << line_prefix
				     << type_str << " "
				     << vP[i].name()
				     << line_suffix << endl;
			}
		}
	}

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
			if (var_style == "in")
			{
				cout << type_str << " ";
			}
		}
		cout << vP[i] << line_suffix << endl;
	}
	return 0;
}
