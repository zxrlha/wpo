#ifndef WPO_YYGLOBAL_HPP
#define WPO_YYGLOBAL_HPP 11

#include "expression.hpp"
#include <boost/variant.hpp>
#include <string>
#include <memory>

using std::string;
using spp = std::shared_ptr<polynomial>;
#define YYSTYPE boost::variant<string, int, monomial, spp>
using boost::get;

extern vector<polynomial> vP;
extern vector<funcexpr> vfunc;
extern vector<int> vindex;

extern vector<string> vtmp_prefix;
extern vector<string> vtmp_suffix;
extern vector<string> vline_prefix;
extern vector<string> vline_suffix;
extern vector<string> vtmp_style;
extern vector<string> vvar_style;
extern vector<string> vinfo_prefix;
extern vector<string> vinfo_suffix;
extern vector<string> vvar_filter;
extern vector<string> voutput_filename;
extern string num_prefix;
extern string num_suffix;
extern string strategy;
extern bool flag_clean;
extern bool flag_reuse;
extern int64_t max_terms;
extern bool multithread;
extern string invname;
extern string logname;
extern string expname;

extern string in_file;

//note: vP_get only works if the vPmap is built correctly, i.e. pushing via vP_push, and modify via vP_replace
//currently only parsing and step 1 called vP_get, and only them using vP_push and vP_replace
void vP_replace(int i, const polynomial& nP);
void vP_rebuild_map();
void vP_push(const polynomial& P);
int vP_insert(polynomial& P);//check before push, will return literal name
int vP_get(const polynomial& P);//return index in vP
void vP_map_remove(const polynomial& P);
int vfunc_insert(funcexpr& fe);//check before push, will return literal name

void parse_options(const string& name, const string& value);
void init_ring_defaults();

void error_end();

void init_mulnum();
int get_mulnum();
void update_mulnum(int step, int bv);

#endif
