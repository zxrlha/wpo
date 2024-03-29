#include <functional>
#include <algorithm>
#include "expression.hpp"
#include "literal.hpp"
#include "yyglobal.hpp"

using std::min;
using std::max;

int monomial::getpow(int i) const
{
    auto it = std::lower_bound(_vterm.begin(), _vterm.end(), i,
                               [](std::pair<int, int> t, int i) -> bool
    {
        return t.first < i;
    });
    if (it != _vterm.end() && it->first == i)
    {
        return it->second;
    }
    else
    {
        return 0;
    }
}

int monomial::ring_pow_sum(int minlvl, int maxlvl) const
{
    int res = 0;
    for (int i = 0; i < _vterm.size(); ++i)
    {
        int lvl = literal_get_ring_level(_vterm[i].first);
        if (lvl >= minlvl && lvl <= maxlvl)
        {
            res += _vterm[i].second;
        }
    }
    return res;
}

monomial monomial::ring_sub_mon(int minlvl, int maxlvl) const
{
    monomial nm;
    for (int i = 0; i < _vterm.size(); ++i)
    {
        int lvl = literal_get_ring_level(_vterm[i].first);
        if (lvl >= minlvl && lvl <= maxlvl)
        {
            nm.append(_vterm[i].first, _vterm[i].second);
        }
    }
    return nm;
}

monomial& monomial::operator*=(const monomial& m2)
{
    vector<std::pair<int, int>> vtmp;
    vtmp.reserve(_vterm.size() + m2._vterm.size());
    int i = 0;
    int j = 0;
    while (i != _vterm.size() && j != m2._vterm.size())
    {
        if (_vterm[i].first < m2._vterm[j].first)
        {
            vtmp.push_back(_vterm[i]);
            ++i;
        }
        else if (m2._vterm[j].first < _vterm[i].first)
        {
            vtmp.push_back(m2._vterm[j]);
            ++j;
        }
        else
        {
            vtmp.push_back(std::make_pair(_vterm[i].first, _vterm[i].second + m2._vterm[j].second));
            ++i;
            ++j;
        }
    }
    if (i == _vterm.size())
    {
        while (j != m2._vterm.size())
        {
            vtmp.push_back(m2._vterm[j]);
            ++j;
        }
    }
    if (j == m2._vterm.size())
    {
        while (i != _vterm.size())
        {
            vtmp.push_back(_vterm[i]);
            ++i;
        }
    }
    _vterm = std::move(vtmp);
    _coef *= m2._coef;
    return *this;
}

monomial& monomial::operator/=(const monomial& m2)
{
    vector<std::pair<int, int>> vtmp;
    vtmp.reserve(_vterm.size());
    int i = 0;
    int j = 0;
    while (i != _vterm.size() && j != m2._vterm.size())
    {
        if (_vterm[i].first < m2._vterm[j].first)
        {
            vtmp.push_back(_vterm[i]);
            ++i;
            continue;
        }
        assert(m2._vterm[j].first == _vterm[i].first);
        assert(_vterm[i].second >= m2._vterm[j].second);
        if (_vterm[i].second != m2._vterm[j].second)
        {
            vtmp.push_back(std::make_pair(_vterm[i].first, _vterm[i].second - m2._vterm[j].second));
        }
        ++i;
        ++j;
    }
    assert(j == m2._vterm.size());
    while (i != _vterm.size())
    {
        vtmp.push_back(_vterm[i]);
        ++i;
    }
    _vterm = std::move(vtmp);
    _coef /= m2._coef;
    return *this;
}

void polynomial::sort()
{
    std::sort(_vmon.begin(), _vmon.end());
}

polynomial& polynomial::operator+=(const monomial& A)
{
    _vmon.insert(std::upper_bound(_vmon.begin(), _vmon.end(), A), A);
    return *this;
}

polynomial& polynomial::operator+=(monomial&& A)
{
    _vmon.insert(std::upper_bound(_vmon.begin(), _vmon.end(), A), std::move(A));
    return *this;
}

polynomial& polynomial::operator-=(const monomial& A)
{
    monomial B(A);
    B.reverse_sign();
    _vmon.insert(std::upper_bound(_vmon.begin(), _vmon.end(), B), std::move(B));
    return *this;
}

polynomial& polynomial::operator-=(monomial&& A)
{
    A.reverse_sign();
    _vmon.insert(std::upper_bound(_vmon.begin(), _vmon.end(), A), std::move(A));
    return *this;
}
polynomial operator+(const monomial& A, const monomial& B)
{
    polynomial P(A);
    P += B;
    return P;
}

polynomial operator-(const monomial& A, const monomial& B)
{
    polynomial P(A);
    P -= B;
    return P;
}

polynomial& polynomial::operator*=(const monomial& A)
{
    for (auto& m : _vmon)
    {
        m *= A;
    }
    return *this;
}

polynomial operator*(const polynomial& P, const monomial& A)
{
    polynomial res(P);
    res *= A;
    return res;
}

polynomial operator+(const polynomial& P, const monomial& A)
{
    polynomial res(P);
    res += A;
    return res;
}

polynomial operator-(const polynomial& P, const monomial& A)
{
    polynomial res(P);
    res -= A;
    return res;
}

monomial operator*(const monomial& A, const monomial& B)
{
    monomial C(A);
    C *= B;
    return C;
}

polynomial operator+(const polynomial& P1, const polynomial& P2)
{
    polynomial res(P1);
    for (int i = 0; i < P2.size(); ++i)
    {
        res += P2[i];
    }
    return res;
}

polynomial operator-(const polynomial& P1, const polynomial& P2)
{
    polynomial res(P1);
    for (int i = 0; i < P2.size(); ++i)
    {
        res -= P2[i];
    }
    return res;
}

bool operator<(const monomial& A, const monomial& B)
{
    if (A.size() < B.size())
    {
        return true;
    }
    if (A.size() > B.size())
    {
        return false;
    }
    if (A.coef() < B.coef())
    {
        return true;
    }
    if (A.coef() > B.coef())
    {
        return false;
    }
    for (int i = 0; i < A.size(); ++i)
    {
        if (A.lit(i) < B.lit(i))
        {
            return true;
        }
        if (A.lit(i) > B.lit(i))
        {
            return false;
        }
        if (A.pow(i) < B.pow(i))
        {
            return true;
        }
        if (A.pow(i) > B.pow(i))
        {
            return false;
        }
    }
    return false;
}

bool operator==(const monomial& A, const monomial& B)
{
    if (A.size() != B.size())
    {
        return false;
    }
    if (A.coef() != B.coef())
    {
        return false;
    }
    for (int i = 0; i < A.size(); ++i)
    {
        if (A.lit(i) != B.lit(i))
        {
            return false;
        }
        if (A.pow(i) != B.pow(i))
        {
            return false;
        }
    }
    return true;
}

bool operator==(const polynomial& A, const polynomial& B)
{
    if (A.size() != B.size())
    {
        return false;
    }
    for (int i = 0; i < A.size(); ++i)
    {
        if (A[i] != B[i])
        {
            return false;
        }
    }
    return true;
}

bool operator<(const polynomial& A, const polynomial& B)
{
    if (A.size() < B.size())
    {
        return true;
    }
    if (A.size() > B.size())
    {
        return false;
    }
    for (int i = 0; i < A.size(); ++i)
    {
        if (A[i] < B[i])
        {
            return true;
        }
        if (A[i] > B[i])
        {
            return false;
        }
    }
    return false;
}

bool polynomial::contain(const monomial& m) const
{
    return std::binary_search(_vmon.begin(), _vmon.end(), m);
}

int monomial::multiplication_number() const
{
    int res = 0;
    for (int i = 0; i < _vterm.size(); ++i)
    {
        res += _vterm[i].second;
    }
    return res;
}

ostream& operator<<(ostream& os, const monomial& m)
{
    if (m.coef() == 1)
    {
    }
    else if (m.coef() == -1)
    {
        os << "-";
    }
    else
    {
        os << m.coef() << "*";
    }
    int status = 0;
    for (int i = 0; i < m.size(); ++i)
    {
        for (int j = 0; j < m.pow(i); ++j)
        {
            if (status != 0)
            {
                os << "*";
            }
            os << literal_name(m.lit(i));
            status = 1;
        }
    }
    if (status == 0)
    {
        os << num_prefix << "1" << num_suffix;
    }
    return os;
}

ostream& operator<<(ostream& os, const polynomial& p)
{
    os << p.name() << "=";
    for (int i = 0; i < p.size(); ++i)
    {
        if (i != 0 && p[i].is_positive())
        {
            os << "+";
        }
        os << p[i];
    }
    return os;
}

void polynomial::remove(const monomial& m)
{
    auto it = std::find(_vmon.begin(), _vmon.end(), m);
    assert(*it == m);
    _vmon.erase(it);
}

monomial polynomial::gcd() const
{
    if (size() == 0)
    {
        return monomial();
    }
    monomial res;
    std::vector<int> ci(_vmon.size(), 0);
    int clit = -1;
    int i = 0;
    int mi = -1;
    while (ci[i] < _vmon[i].size())
    {
        if (_vmon[i].lit(ci[i]) > clit)
        {
            clit = _vmon[i].lit(ci[i]);
            i = 0;
            mi = -1;
        }
        else if (_vmon[i].lit(ci[i]) == clit)
        {
            if (mi != -1)
            {
                mi = std::min(mi, _vmon[i].pow(ci[i]));
            }
            else
            {
                mi = _vmon[i].pow(ci[i]);
            }
            ++i;
            if (i == _vmon.size())//one common factor
            {
                res *= monomial(clit, mi);
                //reset
                i = 0;
                mi = -1;
                ++ci[i];
            }
        }
        else
        {
            ++ci[i];
        }
    }
    return res;
}

bool polynomial::contain_literals(const set<int>& ls)
{
    for (int i = 0; i < _vmon.size(); ++i)
    {
        for (int j = 0; j < _vmon[i].size(); ++j)
        {
            if (ls.count(_vmon[i].lit(j)) != 0)
            {
                return true;
            }
        }
    }
    return false;
}

set<int> polynomial::literals() const
{
    set<int> res;
    for (int i = 0; i < _vmon.size(); ++i)
    {
        for (int j = 0; j < _vmon[i].size(); ++j)
        {
            res.insert(_vmon[i].lit(j));
        }
    }
    return res;
}

set<int> polynomial::tmp_literals() const
{
    set<int> res;
    for (int i = 0; i < _vmon.size(); ++i)
    {
        for (int j = 0; j < _vmon[i].size(); ++j)
        {
            if (literal_is_tmp(_vmon[i].lit(j)))
            {
                res.insert(_vmon[i].lit(j));
            }
        }
    }
    return res;
}

int polynomial::single_id() const
{
    if (_vmon.size() != 1)
    {
        return -1;
    }
    if (_vmon[0].coef() != 1)
    {
        return -1;
    }
    if (_vmon[0].size() != 1)
    {
        return -1;
    }
    if (_vmon[0].pow(0) != 1)
    {
        return -1;
    }
    return _vmon[0].lit(0);
}

polynomial operator/(const polynomial& P, const monomial& A)
{
    polynomial res;
    for (int mi = 0; mi < P.size(); ++mi)
    {
        int i = 0;
        int j = 0;
        bool flag = true;
        while (i != P[mi].size() && j != A.size())
        {
            if (P[mi].lit(i) < A.lit(j))
            {
                ++i;
                continue;
            }
            if (P[mi].lit(i) > A.lit(j))
            {
                flag = false;
                break;
            }
            if (P[mi].pow(i) < A.pow(j))
            {
                flag = false;
                break;
            }
            ++i;
            ++j;
        }
        if (j != A.size()) flag = false;
        if (!flag)
        {
            continue;
        }
        monomial B(P[mi]);
        B /= A;
        res += B;
    }
    return res;
}

int polynomial::multiplication_number() const
{
    int sum = 0;
    for (int i = 0; i < _vmon.size(); ++i)
    {
        sum += std::max(0, _vmon[i].multiplication_number() - 1);
        if (_vmon[i].coef() != 1 && _vmon[i].coef() != -1)
        { ++sum; }
    }
    return sum;
}

int gcd_mn(const monomial& A, const monomial& B, int minlvl, int maxlvl)
{
    int sum = 0;
    int i = 0;
    int j = 0;
    while (i != A.size() && j != B.size())
    {
        if (A.lit(i) < B.lit(j))
        {
            ++i;
        }
        else if (A.lit(i) > B.lit(j))
        {
            ++j;
        }
        else
        {
            int lvl = literal_get_ring_level(A.lit(i));
            if (lvl >= minlvl && lvl <= maxlvl)
            {
                sum += std::min(A.pow(i), B.pow(j));
            }
            ++i;
            ++j;
        }
    }
    return sum;
}

monomial gcd(const monomial& A, const monomial& B, int minlvl, int maxlvl)
{
    monomial res;
    int i = 0;
    int j = 0;
    while (i != A.size() && j != B.size())
    {
        if (A.lit(i) < B.lit(j))
        {
            ++i;
        }
        else if (A.lit(i) > B.lit(j))
        {
            ++j;
        }
        else
        {
            int lvl = literal_get_ring_level(A.lit(i));
            if (lvl >= minlvl && lvl <= maxlvl)
            {
                res.append(A.lit(i), std::min(A.pow(i), B.pow(j)));
            }
            ++i;
            ++j;
        }
    }
    return res;
}

int monomial::ring_level() const
{
    int min = literal_maximum_ring_level();
    for (int i = 0; i < _vterm.size(); ++i)
    {
        int t = literal_get_ring_level(_vterm[i].first);
        if (min > t)
        {
            min = t;
        }
    }
    return min;
}

int polynomial::ring_level() const
{
    int lvl = literal_find_ring_level(name());
    if (lvl != -1) return lvl;
    int min = literal_maximum_ring_level();
    for (int i = 0; i < _vmon.size(); ++i)
    {
        int t = _vmon[i].ring_level();
        if (min > t)
        {
            min = t;
        }
    }
    return min;
}
int funcexpr::ring_level() const
{
    int lvl1 = literal_find_ring_level(_funcname);
    int lvl2 = literal_get_ring_level(_paraid);
    if (lvl1 == -1) lvl1 = literal_get_default_ring_level();
    return std::min(lvl1, lvl2);
}

string monomial::ring_type() const
{
    return literal_get_ring_type(ring_level());
}

string polynomial::ring_type() const
{
    return literal_get_ring_type(ring_level());
}

string funcexpr::ring_type() const
{
    return literal_get_ring_type(ring_level());
}
