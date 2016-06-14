#include <iostream>
#include <fstream>
#include <set>
#include <cstdlib>
#include <cstdio>
#include <boost/program_options.hpp>
#include "kernel.hpp"
#include "extraction.hpp"
#include "kcm.hpp"
#include "literal.hpp"
#include "yyglobal.hpp"
#include "parser.hpp"
#include "output.hpp"
#include "config.h"

using std::cout;
using std::cerr;
using std::endl;
using std::set;
using std::string;

extern int yyparse();
extern FILE* yyin;

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

void output_func(const string& paraname, std::ostream& os)
{
	vector<string> vn;
	for (auto it = vfunc.begin(); it != vfunc.end();)
	{
		if (literal_name(it->_paraid) == paraname)
		{
			os << line_prefix;
			if (func_style == "in")
			{
				os << it->ring_type() << " ";
			}
			os << it->_resname << "="
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
		output_func(str, os);
	}
}

void output_prime_func(std::ostream& os)
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
			os << line_prefix;
			if (func_style == "in")
			{
				os << it->ring_type() << " ";
			}
			os << it->_resname << "="
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
		output_func(str, os);
	}
}

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
	std::cerr.precision(4);
	po::options_description desc("Allowd options");
	desc.add_options()
	("help,h", "print help message")
	("version,v", "print version information")
	("input,i", po::value<std::string>(), "input file")
	("output,o", po::value<std::string>(), "output file")
	("append,a", "append instead of overwrite")
	;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return 0;
	}
	if (vm.count("version"))
	{
		std::cout << PACKAGE_STRING << std::endl;
		return 0;
	}
	if (vm.count("input"))
	{
		yyin = fopen(vm["input"].as<std::string>().c_str(), "r");
		if (!yyin)
		{
			std::cerr << "ERROR: Cannot open input file " << vm["input"].as<std::string>() << std::endl;
			return 1;
		}
	}
	std::ostream* pos;
	if (vm.count("output"))
	{
		auto mode = std::ios::out;
		if (vm.count("append"))
		{
			mode |= std::ios::app;
		}
		std::ofstream* tofs = new std::ofstream(vm["output"].as<std::string>(), mode);
		if (!tofs->is_open())
		{
			std::cerr << "ERROR: Cannot open output file " << vm["output"].as<std::string>() << std::endl;
		}
		pos = tofs;
	}
	else
	{
		pos = &cout;
	}
	std::ostream& os = *pos;
	cerr << "Parsing\r";
	yyparse();
	cerr << "Total " << vP.size() << " polynomials, ";
	for (int i = 0; i < vP.size(); ++i)
	{
		summul += vP[i].multiplication_number();
	}
	osummul = summul;
	cerr << summul << " multiplications" << endl;
	if (!literal_ring_type_check())
	{
		cerr << "Ring type check failed, please check your ring configuration!" << endl;
		return 1;
	}
	find_kernel_intersections(vP);
	find_cube_intersections(vP);
	if (flag_clean)
	{
		clean(vP);
	}
	reorder(vP);
	//cerr << "//reorder finished" << endl;
	int max = rename(vP);
	//cerr << "//rename finished" << endl;
	if (tmp_style == "pre")
	{
		for (int i = 0; i < vP.size(); ++i)
		{
			if (literal_is_tmp(vP[i].name()))
			{
				vP[i].name().erase(vP[i].name().begin());
				os << line_prefix
				   << vP[i].ring_type() << " "
				   << vP[i].name()
				   << line_suffix << endl;
			}
		}
	}
	if (var_style == "pre")
	{
		for (int i = 0; i < vP.size(); ++i)
		{
			if (!literal_is_tmp(vP[i].name()) && pass_filter(vP[i].name()))
			{
				os << line_prefix
				   << vP[i].ring_type() << " "
				   << vP[i].name()
				   << line_suffix << endl;
			}
		}
	}
	if (func_style == "pre")
	{
		for (int i = 0; i < vfunc.size(); ++i)
		{
			os << line_prefix
			   << vfunc[i].ring_type() << " "
			   << vfunc[i]._resname
			   << line_suffix << endl;
		}
	}
	output_prime_func(os);
	set<string> declaredtmp;
	for (int i = 0; i < vP.size(); ++i)
	{
		string name = vP[i].name();
		os << line_prefix;
		if (literal_is_tmp(name))
		{
			if (tmp_style == "in" && declaredtmp.count(name) == 0)
			{
				declaredtmp.insert(name);
				os << vP[i].ring_type() << " ";
			}
			vP[i].name().erase(vP[i].name().begin());
		}
		else
		{
			if (var_style == "in" && pass_filter(vP[i].name()))
			{
				os << vP[i].ring_type() << " ";
			}
		}
		os << vP[i] << line_suffix << endl;
		output_func(vP[i].name(), os);
	}
	if (vm.count("input"))
	{
		fclose(yyin);
	}
	if (vm.count("output"))
	{
		delete pos;
	}
	return 0;
}
