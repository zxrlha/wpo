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
vector<std::pair<std::regex, int>> vidlvl;
int dftlvl = 0;

int literal_find_ring_level(const std::string& name)
{
    std::smatch sm;
    for (auto [reg, lvl]: vidlvl)
    {
        if (std::regex_match(name, sm, reg))
        {
            return lvl;
        }
    }
    return -1;
}
