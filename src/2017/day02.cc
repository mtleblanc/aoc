#include "aoc.hh"
#include <algorithm>
#include <ranges>
#include <sstream>

/* https://adventofcode.com/2017/day/2
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 2;
using Solution = Solution_t<YEAR, DAY>;

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    const auto spreadsheet =
        std::ranges::to<std::vector>(readAllLines(input) | std::views::transform(
                                                               [](const auto& line)
                                                               {
                                                                   std::stringstream ss{line};
                                                                   return readAll<int>(ss);
                                                               }));
    auto part1View = spreadsheet | std::views::transform(
                                       [](const auto& row)
                                       {
                                           auto [min, max] = std::ranges::minmax(row);
                                           return max - min;
                                       });
    auto part1 = std::ranges::fold_left(part1View, 0, std::plus<>());

    auto findEvenlyDivisible = [](const auto& row)
    {
        for (auto [i, n] : std::views::zip(std::views::iota(0), row))
        {
            for (auto m : row | std::views::drop(i + 1))
            {
                if (m % n == 0)
                {
                    return m / n;
                }
                if (n % m == 0)
                {
                    return n / m;
                }
            }
        }
        throw std::invalid_argument("Row did not contain evenly divisible elements");
    };
    auto part2 = std::ranges::fold_left(spreadsheet | std::views::transform(findEvenlyDivisible), 0,
                                        std::plus<>());
    return {part1, part2};
}
} // namespace aoc
