#include "aoc.hh"
#include "rangepolyfill.hh"
#include <algorithm>
#include <ctre.hpp>
#include <set>

/* https://adventofcode.com/2016/day/7
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 7;

namespace
{
bool supportsTLS(const std::string& ip)
{
    bool hypernet{};
    bool seen{};
    // my libc++ doesn't have views::adjacent
    for (auto [a, b, c, d] : adjacent<4>(ip))
    {
        if (a == '[')
        {
            hypernet = true;
            continue;
        }
        if (a == ']')
        {
            hypernet = false;
            continue;
        }
        if (a != b && a == d && b == c)
        {
            if (hypernet)
            {
                return false;
            }
            seen = true;
        }
    }
    return seen;
}

bool supportsSLS(const std::string& ip)
{
    using net_t = std::set<std::pair<char, char>>;
    net_t aba;
    net_t bab;
    net_t* net = &aba;
    // my libc++ doesn't have views::adjacent
    for (auto [a, b, c] : adjacent<3>(ip))
    {
        if (a == '[')
        {
            net = &bab;
            continue;
        }
        if (a == ']')
        {
            net = &aba;
            continue;
        }
        if (a != b && a == c)
        {
            net->insert(std::make_pair(a, b));
        }
    }
    return std::ranges::any_of(aba, [&bab](auto p)
                               { return bab.contains(std::make_pair(p.second, p.first)); });
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto lines = readAllLines(input);

    return {static_cast<size_t>(std::ranges::count_if(lines, supportsTLS)),
            static_cast<size_t>(std::ranges::count_if(lines, supportsSLS))};
}
} // namespace aoc
