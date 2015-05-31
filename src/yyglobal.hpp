#ifndef WPO_YYGLOBAL_HPP_
#define WPO_YYGLOBAL_HPP 11

#include "expression.hpp"
#include <boost/variant.hpp>
#include <string>
using std::string;
using yystype = boost::variant<int, string, monomial, polynomial>;
using boost::get;

#define YYSTYPE yystype

extern vector<polynomial> vP;

extern string tmp_prefix;
extern string tmp_suffix;
extern int tmp_start;
extern string line_suffix;

#endif