#include "aoc.hh"
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
    for (size_t i{}; i < ip.size() - 3; ++i)
    {
        if (ip[i] == '[')
        {
            hypernet = true;
            continue;
        }
        if (ip[i] == ']')
        {
            hypernet = false;
            continue;
        }
        if (ip[i] != ip[i + 1] && ip[i] == ip[i + 3] && ip[i + 1] == ip[i + 2])
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
    for (size_t i{}; i < ip.size() - 2; ++i)
    {
        if (ip[i] == '[')
        {
            net = &bab;
            continue;
        }
        if (ip[i] == ']')
        {
            net = &aba;
            continue;
        }
        if (ip[i] != ip[i + 1] && ip[i] == ip[i + 2])
        {
            net->insert(std::make_pair(ip[i], ip[i + 1]));
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
