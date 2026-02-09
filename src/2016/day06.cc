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

template <> struct SolutionType<YEAR, DAY>
{
    using type = aoc::StringSolution;
};

template <> StringSolution solve<YEAR, DAY>(std::istream& input)
{
    auto lines = readAllLines(input);
    std::string password1;
    std::string password2;
    for (auto i : std::views::iota(0UZ, lines.front().size()))
    {
        std::map<char, int> counts;
        std::ranges::for_each(lines, [&counts, i](auto l) { ++counts[l[i]]; });
        password1.push_back(
            std::ranges::max_element(counts, {}, &std::pair<const char, int>::second)->first);
        password2.push_back(
            std::ranges::min_element(counts, {}, &std::pair<const char, int>::second)->first);
    }
    return {password1, password2};
}
} // namespace aoc
