#include "aoc.hh"
#include "util.hh"
#include <algorithm>
#include <set>

/* https://adventofcode.com/2018/day/5
 */
namespace aoc
{
constexpr size_t YEAR = 2018;
constexpr size_t DAY = 5;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
auto part1(auto&& sequence)
{
    auto reduced = std::string{};
    for (const auto& c : sequence)
    {
        if (reduced.empty() || c != (reduced.back() ^ 'A' ^ 'a'))
        {
            reduced.push_back(c);
        }
        else
        {
            reduced.pop_back();
        }
    }
    return std::ssize(reduced);
}

auto part2(std::string_view sequence)
{
    auto units = std::ranges::to<std::set>(
        sequence | std::views::transform([](auto c) { return std::tolower(c); }));
    auto without = [&sequence](auto c)
    { return part1(sequence | std::views::filter([c](auto d) { return std::tolower(d) != c; })); };
    return std::ranges::min(units | std::views::transform(without));
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto sequence = std::string{trim(slurp(input))};
    return {static_cast<int>(part1(sequence)), static_cast<int>(part2(sequence))};
    ;
}
} // namespace aoc
