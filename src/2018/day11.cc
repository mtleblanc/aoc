#include "aoc.hh"
#include <algorithm>
#include <ranges>

/* https://adventofcode.com/2018/day/11
 */
namespace aoc
{
constexpr size_t YEAR = 2018;
constexpr size_t DAY = 11;

template <> struct SolutionType<YEAR, DAY>
{
    using type = StringSolution;
};
using Solution = Solution_t<YEAR, DAY>;

namespace
{
auto powerLevel(auto x, auto y, auto seed)
{
    // NOLINTBEGIN(*-magic-numbers)
    auto rack = x + 10;
    auto pl = (rack * y + seed) * rack;
    return (pl / 100) % 10 - 5;
    // NOLINTEND(*-magic-numbers)
}

constexpr auto GRID = 300;
auto makePartials(auto seed)
{
    // coords range 1..GRID, while partials ranges 0..GRID to make inclusion exclusion caseless
    auto coords =
        std::views::cartesian_product(std::views::iota(1, GRID + 1), std::views::iota(1, GRID + 1));
    std::array<std::array<int, GRID + 1>, GRID + 1> partials{};
    for (auto [x, y] : coords)
    {
        // first pass each entry is partial sum across rows
        partials[x][y] = partials[x - 1][y] + powerLevel(x, y, seed);
    }
    for (auto [x, y] : coords)
    {
        // second pass each entry is partial sum of all elements up and left
        partials[x][y] += partials[x][y - 1];
    }
    return partials;
}

auto optimize(const auto& partials)
{
    using namespace std::views;
    auto powerAt = [&partials](auto s)
    {
        // includsion exclusion:
        //   BAAAA
        //   BAAAA
        //   CDDDD
        //   CDDDD
        //   CDDDD
        // sum of D is sum of all values, minus the values left (C + B), minus the values above (A +
        // B), but B has been subtracted twice, so need to add back once
        auto [x, y, n] = s;
        return partials[x + n - 1][y + n - 1] + partials[x - 1][y - 1] -
               partials[x - 1][y + n - 1] - partials[x + n - 1][y - 1];
    };

    StringSolution res;
    {
        // NB GRID - 1, max top left corner for a 3x3 = GRID - 2
        auto squares = cartesian_product(iota(1, GRID - 1), iota(1, GRID - 1), single(3));
        auto [x, y, n] = std::ranges::max(squares, {}, powerAt);
        res.part1 = std::format("{},{}", x, y);
    }
    {
        auto valid = [](auto square)
        {
            auto [x, y, n] = square;
            return x + n <= GRID + 1 && y + n <= GRID + 1;
        };
        auto squares = cartesian_product(iota(1, GRID + 1), iota(1, GRID + 1), iota(1, GRID + 1)) |
                       filter(valid);
        auto [x, y, n] = std::ranges::max(squares, {}, powerAt);
        res.part2 = std::format("{},{},{}", x, y, n);
    }
    return res;
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto seed = 0;
    input >> seed;
    auto partials = makePartials(seed);
    return optimize(partials);
}
} // namespace aoc
