#ifndef WPO_LITERAL_HPP
#define WPO_LITERAL_HPP 1

#include <string>

using std::string;

//append a new literal, its ring level will be set into 0, unless it is specifical defined in ringid
int literal_append(const string& name);
//append a new tmp literal, its ring level will be set into 0
int literal_append_tmp();
//get the name of this literal
string literal_name(int i);
//return the number of literals
int literal_size();

void literal_set_name(int i, const string& name);

bool literal_check(const string& name);
int literal_add(const string& name, bool num_flag = false);
int literal_get(const string& name);
bool literal_is_tmp(const string& name);
bool literal_is_tmp(int i);

void literal_set_ring_level(int i, int lvl);
int literal_get_ring_level(int i);
int literal_maximum_ring_level();
string literal_get_ring_type(int i);
int literal_find_ring_level(const string& name);

void literal_parse_ring(const string& name, const string& value);

bool literal_ring_type_check();

#endif
