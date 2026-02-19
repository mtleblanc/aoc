#include "aoc.hh"
#include <numeric>
#include <ranges>

/* https://adventofcode.com/2015/day/1
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 1;

template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    auto steps = slurp(input);
    auto v = steps | std::views::transform([](auto c) { return c == '(' ? 1 : -1; });
    std::vector<int> floors;
    std::partial_sum(v.begin(), v.end(), back_inserter(floors));
    auto firstBasement = std::ranges::find(floors, -1);
    return {floors.back(), firstBasement - floors.begin() + 1};
}
} // namespace aoc
