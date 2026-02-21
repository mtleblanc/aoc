#include "aoc.hh"
#include "knothash.hh"
#include "util.hh"
#include <ranges>

/* https://adventofcode.com/2017/day/10
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 10;

template <> struct SolutionType<YEAR, DAY>
{
    using type = StringSolution;
};
using Solution = Solution_t<YEAR, DAY>;

namespace
{
int part1(std::string_view text)
{
    auto part1Lengths = readNumbers(text);
    auto part1Hash = hash(part1Lengths);
    return part1Hash[0] * part1Hash[1];
}

std::string part2(std::string_view text)
{
    auto lengths = trim(text) | std::views::transform([](auto c) -> int { return c; }) |
                   std::ranges::to<std::vector>();
    auto dense = denseHash(lengths) | std::views::transform([](auto n) { return toHex(n, 2); });
    return std::ranges::fold_left(dense, std::string{}, std::plus<>());
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto text = slurp(input);
    return {std::to_string(part1(text)), part2(text)};
}
} // namespace aoc
