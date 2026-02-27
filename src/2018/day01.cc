#include "aoc.hh"
#include <algorithm>
#include <set>

/* https://adventofcode.com/2018/day/1
 */
namespace aoc
{
constexpr size_t YEAR = 2018;
constexpr size_t DAY = 1;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
auto part2(const auto& deltas)
{
    auto total = 0;
    auto seen = std::set<int>{};
    for (;;)
    {
        for (auto d : deltas)
        {
            total += d;
            if (seen.contains(total))
            {
                return total;
            }
            seen.insert(total);
        }
    }
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto deltas = readAll<int>(input);
    auto part1 = std::ranges::fold_left(deltas, 0, std::plus<>());
    return {part1, part2(deltas)};
}
} // namespace aoc
