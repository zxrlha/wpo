#ifndef WPO_YYGLOBAL_HPP_
#define WPO_YYGLOBAL_HPP 11

#include "expression.hpp"
#include <boost/variant.hpp>
#include <string>
using std::string;
using yystype = boost::variant<int64_t, string, monomial, polynomial>;
using boost::get;

#define YYSTYPE yystype

extern vector<polynomial> vP;
extern vector<funcexpr> vfunc;
extern vector<int> vindex;

extern string tmp_prefix;
extern string tmp_suffix;
extern int tmp_start;
extern string line_prefix;
extern string line_suffix;
extern string type_str;
extern string tmp_style;
extern string var_style;
extern string var_filter;
extern string func_prefix;
extern string func_style;
extern string strategy;
extern bool flag_clean;
extern bool flag_reuse;

extern int64_t summul;
extern int64_t osummul;

int vP_get(const polynomial& P);
int vfunc_get(const funcexpr& fe);

void parse_options(const string& name, const string& value);
#endif
