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
auto powerLevel(auto p, auto seed)
{
    auto [x, y] = p;
    // NOLINTBEGIN(*-magic-numbers)
    auto rackId = x + 10;
    auto powerLevel = (rackId * y + seed) * rackId;
    return (powerLevel / 100) % 10 - 5;
    // NOLINTEND(*-magic-numbers)
}

auto operator+(std::tuple<int, int> l, std::tuple<int, int> r)
{
    return std::make_pair(std::get<0>(l) + std::get<0>(r), std::get<1>(l) + std::get<1>(r));
}

constexpr auto GRID = 300;
auto part1(auto seed)
{
    auto coords =
        std::views::cartesian_product(std::views::iota(2, GRID), std::views::iota(2, GRID));
    auto totalPower = [seed](auto p)
    {
        auto res = 0;
        for (auto dx : {-1, 0, 1})
        {
            for (auto dy : {-1, 0, 1})
            {
                res += powerLevel(p + std::make_pair(dx, dy), seed);
            }
        }
        return res;
    };
    auto [x, y] = std::ranges::max(coords, {}, totalPower);
    return std::to_string(x - 1) + ',' + std::to_string(y - 1);
}

auto part2(auto seed)
{
    // coords range 1..GRID, while partials ranges 0..GRID to make inclusion exclusion caseless
    auto coords =
        std::views::cartesian_product(std::views::iota(1, GRID + 1), std::views::iota(1, GRID + 1));
    std::array<std::array<int, GRID + 1>, GRID + 1> partials{};
    for (auto p : coords)
    {
        // first pass each entry is partial sum across rows
        auto [x, y] = p;
        partials[x][y] = partials[x - 1][y] + powerLevel(p, seed);
    }
    for (auto [x, y] : coords)
    {
        // second pass each entry is partial sum of all elements up and left
        partials[x][y] += partials[x][y - 1];
    }
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

    auto best = std::ranges::max(squares, {},
                                 [&partials](auto s) -> int
                                 {
                                     auto [c, n] = s;
                                     auto [x, y] = c;
                                     return partials[x + n - 1][y + n - 1] +
                                            partials[x - 1][y - 1] - partials[x - 1][y + n - 1] -
                                            partials[x + n - 1][y - 1];
                                 });
    auto [c, n] = best;
    auto [x, y] = c;

    return std::to_string(x) + ',' + std::to_string(y) + ',' + std::to_string(n);
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto seed = 0;
    input >> seed;
    return {part1(seed), part2(seed)};
}
} // namespace aoc
