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
    auto rackId = x + 10;
    auto powerLevel = (rackId * y + seed) * rackId;
    return (powerLevel / 100) % 10 - 5;
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
    auto coords =
        std::views::cartesian_product(std::views::iota(1, GRID + 1), std::views::iota(1, GRID + 1));
    auto squares =
        coords |
        std::views::transform(
            [](auto c)
            {
                auto m = std::max(std::get<0>(c), std::get<1>(c));
                return std::views::iota(0, GRID + 1 - m) |
                       std::views::transform([c](auto n) { return std::make_pair(c, n); });
            }) |
        std::views::join;

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
        auto [c, n] = s;
        auto [x, y] = c;
        return partials[x + n - 1][y + n - 1] + partials[x - 1][y - 1] -
               partials[x - 1][y + n - 1] - partials[x + n - 1][y - 1];
    };

    StringSolution res;
    {
        auto sizeThree = squares | std::views::filter([](auto s) { return s.second == 3; });
        auto best = std::ranges::max(sizeThree, {}, powerAt);
        auto [c, n] = best;
        auto [x, y] = c;
        res.part1 = std::format("{},{}", x, y);
    }
    {
        auto best = std::ranges::max(squares, {}, powerAt);
        auto [c, n] = best;
        auto [x, y] = c;
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
