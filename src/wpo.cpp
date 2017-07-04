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

bool pass_filter(const std::string& name, int rl)
{
    if (name.find_first_of(vvar_filter[rl]) != std::string::npos)
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
        cout << vP[i] << endl;
    }
}
void debug_out(vector<vector<int>>& vorder)
{
    for (int i = 0; i < vorder.size(); ++i)
    {
        for (int j = 0; j < vorder[i].size(); ++j)
        {
            if (vorder[i][j] < 0)
            {
                cout << vline_prefix[i];
                cout << literal_get_ring_type(i) << " " << vfunc[-vorder[i][j] - 1]._resname << "=" << vfunc[-vorder[i][j] - 1]._funcname << "(" << literal_name(vfunc[-vorder[i][j] - 1]._paraid) << ")" << vline_suffix[i] << endl;
            }
            else
            {
                cout << vline_prefix[i];
                cout << literal_get_ring_type(i) << " " << vP[vorder[i][j]] << vline_suffix[i] << endl;
            }
        }
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
    ("append,a", "append to the output file instead of overwrite it")
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
        in_file = vm["input"].as<std::string>();
        yyin = fopen(in_file.c_str(), "r");
        if (!yyin)
        {
            std::cerr << "ERROR: Cannot open input file " << in_file << std::endl;
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
            return 1;
        }
        pos = tofs;
    }
    else
    {
        pos = &cout;
    }
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
        cerr << "Ring type check failed, please check your declaration for ringtype!" << endl;
        return 1;
    }
    init_ring_defaults();
    find_intersections(vP);
    if (flag_clean)
    {
        clean();
    }
    vector<vector<int>> vorder;
    reorder(vorder);
    //cerr << "//reorder finished" << endl;
    vector<int> vmax;
    rename(vorder, vmax);
    //cerr << "//rename finished" << endl;
    //now output
    for (int rl = 0; rl < vorder.size(); ++rl)
    {
        ostream* tpos;
        if (voutput_filename.size() == 0)//use output specified from command line
        {
            tpos = pos;
        }
        else
        {
            assert(voutput_filename.size() > rl);
            auto mode = std::ios::out;
            if (vm.count("append"))
            {
                mode |= std::ios::app;
            }
            std::ofstream* vrlos = new std::ofstream(voutput_filename[rl], mode);
            if (!vrlos->is_open())
            {
                std::cerr << "ERROR: Cannot open output file " << vm["output"].as<std::string>() << std::endl;
                return 1;
            }
            tpos = vrlos;
        }
        ostream& os = *tpos;
        //first information
        os<<vinfo_prefix[rl]<<vmax[rl]+1<<vinfo_suffix[rl]<<std::endl;
        //pre declarations
        for (int i = 0; i < vorder[rl].size(); ++i)
        {
            int index = vorder[rl][i];
            if (index < 0)//functions
            {
                index = -1 - index;
                assert(literal_is_tmp(vfunc[index]._resname));
                if (vtmp_style[rl] == "pre")
                {
                    vfunc[index]._resname.erase(vfunc[index]._resname.begin());
                    os << vline_prefix[rl]
                       << vfunc[index].ring_type() << " "
                       << vfunc[index]._resname
                       << vline_suffix[rl] << endl;
                }
            }
            else
            {
                if (vtmp_style[rl] == "pre" && literal_is_tmp(vP[index].name()))
                {
                    vP[index].name().erase(vP[index].name().begin());
                    os << vline_prefix[rl]
                       << vP[index].ring_type() << " "
                       << vP[index].name()
                       << vline_suffix[rl] << endl;
                }
                else if (vvar_style[rl] == "pre" && pass_filter(vP[index].name(), rl))
                    os << vline_prefix[rl]
                       << vP[i].ring_type() << " "
                       << vP[i].name()
                       << vline_suffix[rl] << endl;
            }
        }
        //then output them in order
        set<string> declaredtmp;
        for (int i = 0; i < vorder[rl].size(); ++i)
        {
            int index = vorder[rl][i];
            if (index < 0)//functions
            {
                index = -1 - index;
                if (vtmp_style[rl] != "pre")//for in,null,we may need to declare it here
                {
                    assert(literal_is_tmp(vfunc[index]._resname));
                    vfunc[index]._resname.erase(vfunc[index]._resname.begin());
                }
                os << vline_prefix[rl];
                if (vtmp_style[rl] == "in" && declaredtmp.count(vfunc[index]._resname) == 0)
                {
                    declaredtmp.insert(vfunc[index]._resname);
                    os << vfunc[index].ring_type() << " ";
                }
                os << vfunc[index]._resname << "="
                   << vfunc[index]._funcname << "(" << literal_name(vfunc[index]._paraid) << ")"
                   << vline_suffix[rl] << endl;
            }
            else
            {
                os << vline_prefix[rl];
                if (literal_is_tmp(vP[index].name()))
                {
                    if (vtmp_style[rl] == "in" && declaredtmp.count(vP[index].name()) == 0)
                    {
                        declaredtmp.insert(vP[index].name());
                        os << vP[index].ring_type() << " ";
                    }
                    vP[index].name().erase(vP[index].name().begin());
                }
                else if (vvar_style[rl] == "in" && pass_filter(vP[index].name(), rl))
                {
                    os << vP[index].ring_type() << " ";
                }
                os << vP[index] << vline_suffix[rl] << endl;
            }
        }
        if (voutput_filename.size() != 0)//use output specified from command line
        {
            delete tpos;
        }
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
