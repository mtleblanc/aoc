#include "aoc.hh"
#include "util.hh"
#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <stdexcept>

/* https://adventofcode.com/2018/day/6
 */
namespace aoc
{
constexpr size_t YEAR = 2018;
constexpr size_t DAY = 6;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
auto parse(std::string_view text)
{
    auto numbers = readNumbers(text);
    if (numbers.size() != 2)
    {
        throw std::invalid_argument(std::format("Malformed input, expected 2 numbers {}", text));
    }
    return std::make_pair(numbers[0], numbers[1]);
}

// TODO: There's no performance issue, so a lot of the ugliness could be cleaned up:
//   * Infinite detection may be incorrect on some inputs
//   * Grid marking relying on magic numbers
//   * Handling infinite area by just setting it to INT_MIN so it's excluded
//
// TODO: Possible to compute this directly from coordinates instead of iterating through grid points
//
auto part1(const auto& coords)
{
    auto [l, r] = std::ranges::minmax(coords | std::views::keys);
    auto [t, b] = std::ranges::minmax(coords | std::views::values);
    static constexpr auto GUTTER = 1;
    l -= GUTTER;
    r += GUTTER;
    t -= GUTTER;
    b += GUTTER;
    auto stride = r - l + 1;
    auto height = b - t + 1;
    auto grid = std::vector<std::pair<int, int>>(stride * height);
    auto areas = std::vector<int>(coords.size());

    auto frontier = std::deque<std::pair<int, int>>{};
    for (const auto& [i, c] : std::views::zip(std::views::iota(0), coords))
    {
        auto [x, y] = c;
        x -= l;
        y -= t;
        // not checking if duplicate coords were provided, second one just overwrites
        grid[x + y * stride] = {i, 1};
        frontier.emplace_back(x, y);
    }

    while (frontier.size())
    {
        auto [x, y] = frontier.front();
        frontier.pop_front();
        auto [val, n] = grid[x + y * stride];
        if (val == -1)
        {
            // location was equidistant
            continue;
        }
        ++areas[val];
        static constexpr std::array<std::pair<int, int>, 4> DIRS = {
            {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};
        for (auto [dx, dy] : DIRS)
        {
            auto nx = x + dx;
            auto ny = y + dy;
            if (nx < 0 || ny < 0 || nx >= stride || ny >= height)
            {
                // if an area reaches grid edge, assume infinite
                areas[val] = std::numeric_limits<int>::min();
                continue;
            }
            auto& [nval, m] = grid[nx + ny * stride];
            if (m == 0)
            {
                nval = val;
                m = n + 1;
                frontier.emplace_back(nx, ny);
            }
            else if (m == n + 1 && nval != val)
            {
                nval = -1;
            }
        }
    }

    return std::ranges::max(areas);
}

// this could be done much faster but it runs in 4ms by brute force
// TODO: x and y can be treated separately, and the total x (resp y) can be found incrementally by
// just noting how many x coordinates are to the left and right.  The values will be monotone
// decreaasing until a minimum, then monotone increasing.  So we could compute the area just walking
// the convex border
auto part2(const auto& coords)
{
    static constexpr auto DIST = 10000;
    auto [l, r] = std::ranges::minmax(coords | std::views::keys);
    auto [t, b] = std::ranges::minmax(coords | std::views::values);

    auto res = 0;
    for (auto x : std::views::iota(l, r + 1))
    {
        for (auto y : std::views::iota(t, b + 1))
        {
            auto accum = 0;
            for (const auto& [cx, cy] : coords)
            {
                accum += std::abs(x - cx) + std::abs(y - cy);
            }
            if (accum <= DIST)
            {
                ++res;
            }
        }
    }
    return res;
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto coords = std::ranges::to<std::vector>(readAllLines(input) | std::views::transform(parse));
    return {part1(coords), part2(coords)};
}
} // namespace aoc
