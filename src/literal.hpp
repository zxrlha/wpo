#ifndef WPO_LITERAL_HPP
#define WPO_LITERAL_HPP 1

#include <string>

using std::string;

int literal_append(const string& name);
int literal_append_tmp();
string literal_name(int i);
int literal_size();

void literal_set_name(int i, const string& name);

bool literal_check(const string& name);
int literal_add(const string& name);
int literal_get(const string& name);
bool literal_is_tmp(const string& name);
bool literal_is_tmp(int i);
#endif
