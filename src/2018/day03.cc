#include "aoc.hh"
#include "util.hh"
#include <algorithm>
#include <cstddef>
#include <format>
#include <stdexcept>

/* https://adventofcode.com/2018/day/3
 */
namespace aoc
{
constexpr size_t YEAR = 2018;
constexpr size_t DAY = 3;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
struct Claim
{
    int id;
    int l;
    int t;
    int w;
    int h;
};

Claim parse(std::string_view sv)
{
    enum Params
    {
        ID,
        LEFT,
        TOP,
        WIDTH,
        HEIGHT,
        COUNT
    };
    auto numbers = readNumbers(sv);
    if (numbers.size() != COUNT)
    {
        throw std::invalid_argument(
            std::format("Malformed input, expected {} numbers {}", static_cast<int>(COUNT), sv));
    }
    return Claim{numbers[ID], numbers[LEFT], numbers[TOP], numbers[WIDTH], numbers[HEIGHT]};
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    using namespace std::views;
    auto claims = std::ranges::to<std::vector>(readAllLines(input) | transform(parse));
    auto width = std::ranges::max(claims | transform([](const auto& c) { return c.w + c.l; }));
    auto height = std::ranges::max(claims | transform([](const auto& c) { return c.h + c.t; }));
    auto top = std::ranges::min(claims | transform(&Claim::t));
    auto left = std::ranges::min(claims | transform(&Claim::l));

    std::ranges::for_each(claims,
                          [top, left](auto& c)
                          {
                              c.t -= top;
                              c.l -= left;
                          });

    auto grid = std::vector<int>(static_cast<size_t>(width * height));

    for (auto& c : claims)
    {
        for (auto dy : iota(0, c.h))
        {
            for (auto dx : iota(0, c.w))
            {
                ++grid[c.l + dx + width * (c.t + dy)];
            }
        }
    }

    auto part1 = std::ranges::count_if(grid, [](auto v) { return v > 1; });
    auto part2 = std::optional<int>{};

    for (auto& c : claims)
    {
        for (auto dy : iota(0, c.h))
        {
            for (auto dx : iota(0, c.w))
            {
                if (grid[c.l + dx + width * (c.t + dy)] > 1)
                {
                    goto overlap;
                }
            }
        }
        part2 = c.id;
        break;
    overlap:;
    }

    return {static_cast<int>(part1), *part2};
}
} // namespace aoc
