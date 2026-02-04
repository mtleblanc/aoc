#include "aoc.hh"
#include <algorithm>
#include <iostream>
#include <map>
#include <ranges>

/* https://adventofcode.com/2016/day/6
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 6;

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto lines = readAllLines(input);
    std::string password1;
    std::string password2;
    for (auto i : std::views::iota(0UL, lines.front().size()))
    {
        std::map<char, size_t> counts;
        std::ranges::for_each(lines, [&counts, i](auto l) { ++counts[l[i]]; });
        password1.push_back(
            std::ranges::max_element(counts, {}, &std::pair<const char, size_t>::second)->first);
        password2.push_back(
            std::ranges::min_element(counts, {}, &std::pair<const char, size_t>::second)->first);
    }
    std::cout << password1 << ", " << password2 << std::endl;
    return {};
}
} // namespace aoc
