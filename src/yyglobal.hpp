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
extern int64_t threads_num;
extern string invname;
extern string logname;
extern string expname;

extern string in_file;

extern int64_t summul;
extern int64_t osummul;

//note: vP_get only works if the vPmap is built correctly, i.e. pushing via vP_push, and modify via vP_replace
//currently only parsing and step 1 called vP_get, and only them using vP_push and vP_replace
void vP_replace(int i, const polynomial& nP);
void vP_rebuild_map();
void vP_push(const polynomial& P);
int vP_get(polynomial& P, bool create = false);//if not exists and create is set to true, it will call vP_push and create one
void vP_map_remove(const polynomial& P);
int vfunc_get(funcexpr& fe, bool create = false);

void parse_options(const string& name, const string& value);
void parse_options(const string& name, int64_t value);
void init_ring_defaults();

void error_end();

#endif
