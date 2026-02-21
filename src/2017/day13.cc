#include "aoc.hh"
#include "util.hh"
#include <algorithm>
#include <ranges>
#include <stdexcept>

/* https://adventofcode.com/2017/day/13
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 13;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
struct Scanner
{
    int layer;
    int range;

    Scanner(std::span<const int> vals)
    {
        if (vals.size() != 2)
        {
            throw std::invalid_argument("Malformed input");
        }
        layer = vals[0];
        range = vals[1];
    }

    [[nodiscard]] int severity() const
    {
        return layer * range;
    }

    [[nodiscard]] bool caught(int delay = 0) const
    {
        return (layer + delay) % ((range - 1) * 2) == 0;
    }
};
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto scanners = readAllLines(input) |
                    std::views::transform([](const auto& s) { return Scanner{readNumbers(s)}; }) |
                    std::ranges::to<std::vector>();

    auto part1 = std::ranges::fold_left(
        scanners | std::views::filter([](const auto& s) { return s.caught(); }) |
            std::views::transform([](const auto& s) { return s.severity(); }),
        0, std::plus<>());

    auto escape = [&scanners](auto n)
    { return std::ranges::none_of(scanners, [n](const auto& s) { return s.caught(n); }); };
    auto searchSpace = std::views::iota(0);
    auto part2 = std::ranges::find_if(searchSpace, escape);

    return {part1, *part2};
}
} // namespace aoc
