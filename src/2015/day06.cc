#include "aoc.hh"
#include <algorithm>
#include <cassert>
#include <functional>
#include <regex>

/* https://adventofcode.com/2015/day/6
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 6;

namespace
{
template <size_t GridSize> struct Grid
{
    std::array<int, GridSize * GridSize> lights{};

    int& operator()(size_t x, size_t y)
    {
        return lights[y * GridSize + x];
    }

    Grid& applyToRect(size_t x1, size_t y1, size_t x2, size_t y2, const std::function<int(int)>& f)
    {
        if (x1 > x2)
        {
            std::swap(x1, x2);
        }
        if (y1 > y2)
        {
            std::swap(x1, x2);
        }
        for (size_t y = y1; y <= y2; ++y)
        {
            for (size_t x = x1; x <= x2; ++x)
            {
                (*this)(x, y) = f((*this)(x, y));
            }
        }
        return *this;
    }
};

int turnOn(int _)
{
    return 1;
}
int turnOff(int _)
{
    return 0;
}
int toggle(int x)
{
    return 1 - x;
}

int turnUp(int x)
{
    return x + 1;
}
int turnDown(int x)
{
    return x > 0 ? x - 1 : 0;
}
int toggleUp(int x)
{
    return x + 2;
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
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
        auto x1 = stoul(match[X1_INDEX]);
        auto y1 = stoul(match[Y1_INDEX]);
        auto x2 = stoul(match[X2_INDEX]);
        auto y2 = stoul(match[Y2_INDEX]);
        int (*f)(int) {};
        int (*g)(int) {};
        if (match[ACTION_INDEX] == "turn on")
        {
            f = turnOn;
            g = turnUp;
        }
        else if (match[ACTION_INDEX] == "turn off")
        {
            f = turnOff;
            g = turnDown;
        }
        else
        {
            f = toggle;
            g = toggleUp;
        }
        g1.applyToRect(x1, y1, x2, y2, f);
        g2.applyToRect(x1, y1, x2, y2, g);
    }

    return {static_cast<size_t>(std::ranges::fold_left(g1.lights, 0, std::plus<>())),
            static_cast<size_t>(std::ranges::fold_left(g2.lights, 0, std::plus<>()))};
}
} // namespace aoc
