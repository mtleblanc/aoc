#include "aoc.hh"
#include <algorithm>
#include <cassert>
#include <ranges>

/* https://adventofcode.com/2015/day/8
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 8;

namespace
{
ssize_t storedSavings(const std::string& literal)
{
    ssize_t excess = 2; // Start and end quotes
    for (size_t idx = 0; idx < literal.size(); ++idx)
    {
        if (literal[idx] == '\\')
        {
            switch (literal[++idx])
            {
            case '\\': // fallthrough
            case '\"':
                ++excess;
                break;
            case 'x':
                excess += 3;
                idx += 2;
                break;
            default:
                assert(false);
            }
        }
    }
    return excess;
}

ssize_t encodeCost(const std::string& literal)
{
    return 2 + std::ranges::count_if(literal, [](auto c) { return c == '\"' || c == '\\'; });
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto lines = readAllLines(input);

    return {std::ranges::fold_left(lines | std::views::transform(storedSavings), 0, std::plus<>()),
            std::ranges::fold_left(lines | std::views::transform(encodeCost), 0, std::plus<>())};
}
} // namespace aoc
