#include "aoc.hh"
#include <algorithm>
#include <cassert>
#include <functional>
#include <regex>
#include <stdexcept>

/* https://adventofcode.com/2015/day/6
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 6;

namespace
{
struct Rect
{
    size_t x1;
    size_t y1;
    size_t x2;
    size_t y2;
    Rect(size_t x1, size_t y1, size_t x2, size_t y2) : x1{x1}, y1{y1}, x2{x2}, y2{y2}
    {
        if (x1 > x2)
        {
            std::swap(x1, x2);
        }
        if (y1 > y2)
        {
            std::swap(y1, y2);
        }
    }
};
template <size_t GridSize> struct Grid
{
    std::array<int, GridSize * GridSize> lights{};

    int& operator()(size_t x, size_t y)
    {
        return lights[y * GridSize + x];
    }

    Grid& applyToRect(const Rect& r, auto f)
    {
        for (size_t y = r.y1; y <= r.y2; ++y)
        {
            for (size_t x = r.x1; x <= r.x2; ++x)
            {
                (*this)(x, y) = f((*this)(x, y));
            }
        }
        return *this;
    }
};
} // namespace

template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    constexpr size_t GRID_SIZE = 1000;
    auto lines = readAllLines(input);
    std::regex matcher{R"((turn on|turn off|toggle) (\d+),(\d+) through (\d+),(\d+))"};
    constexpr size_t ACTION_INDEX = 1;
    constexpr size_t X1_INDEX = 2;
    constexpr size_t Y1_INDEX = 3;
    constexpr size_t X2_INDEX = 4;
    constexpr size_t Y2_INDEX = 5;
    std::smatch match;
    Grid<GRID_SIZE> g1;
    Grid<GRID_SIZE> g2;
    for (const auto& line : lines)
    {
        std::regex_match(line, match, matcher);
        if (!match.ready() || match.empty())
        {
            throw std::invalid_argument("malformed input");
        }
        Rect r{stoul(match[X1_INDEX]), stoul(match[Y1_INDEX]), stoul(match[X2_INDEX]),
               stoul(match[Y2_INDEX])};

        if (match[ACTION_INDEX] == "turn on")
        {
            g1.applyToRect(r, [](int) { return 1; });
            g2.applyToRect(r, [](int x) { return x + 1; });
        }
        else if (match[ACTION_INDEX] == "turn off")
        {
            g1.applyToRect(r, [](int) { return 0; });
            g2.applyToRect(r, [](int x) { return x > 0 ? x - 1 : 0; });
        }
        else
        {
            g1.applyToRect(r, [](int x) { return 1 - x; });
            g2.applyToRect(r, [](int x) { return x + 2; });
        }
    }

    return {std::ranges::fold_left(g1.lights, 0, std::plus<>()),
            std::ranges::fold_left(g2.lights, 0, std::plus<>())};
}
} // namespace aoc
