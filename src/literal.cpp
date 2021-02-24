#include <cstdlib>
#include <vector>
#include <map>
#include <boost/lexical_cast.hpp>
#include "literal.hpp"
#include "yyglobal.hpp"

using std::vector;
using std::map;

    vector<string> vlit;
    map<string, int> vlit_map;
    vector<int> vlitlvl;
    int tmpi = 0;
    vector<string> vrtype{"auto"};
    map<string, int> vidlvl;
    int dftlvl = 0;
