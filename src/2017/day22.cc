#include "aoc.hh"
#include "util.hh"
#include <algorithm>
#include <cstddef>
#include <map>
#include <ranges>
#include <stdexcept>
#include <utility>

/* https://adventofcode.com/2017/day/22
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 22;
using Solution = Solution_t<YEAR, DAY>;

namespace
{

// best performance seems to be in the 128-512 range
constexpr auto GRID_SIZE = 128;

auto& operator+=(std::pair<int, int>& loc, std::pair<int, int> dir)
{
    auto& [lx, ly] = loc;
    auto [dx, dy] = dir;
    lx += dx;
    ly += dy;
    return loc;
}

auto left(std::pair<int, int> dir)
{
    auto [x, y] = dir;
    return std::make_pair(y, -x);
}

auto right(std::pair<int, int> dir)
{
    auto [x, y] = dir;
    return std::make_pair(-y, x);
}

auto rev(std::pair<int, int> dir)
{
    auto [x, y] = dir;
    return std::make_pair(-x, -y);
}

template <int SZ> struct Grid
{
    std::array<int, static_cast<std::size_t>(SZ* SZ)> cells{};
    int& operator[](std::pair<int, int> coord)
    {
        auto [x, y] = coord;
        return cells[x + SZ * y];
    }
};

template <int SZ> class InfiniteGrid
{
    std::map<std::pair<int, int>, Grid<SZ>> grids;
    std::pair<int, int> cur_;
    std::pair<int, int> curGrid_;
    Grid<SZ>* local = nullptr;

    friend auto parse(const auto& lines, int infected);
    int& operator[](std::pair<int, int> coord)
    {
        auto [x, y] = coord;
        auto [gx, dx] = divmod(x, SZ);
        auto [gy, dy] = divmod(y, SZ);
        return grids[{gx, gy}][{dx, dy}];
    }

  public:
    InfiniteGrid() = default;
    int& cur()
    {
        return (*local)[cur_];
    }

    void updateLocal()
    {
        auto [x, y] = cur_;
        auto [gx, dx] = divmod(x, SZ);
        auto [gy, dy] = divmod(y, SZ);
        curGrid_ += std::make_pair(gx, gy);
        cur_ = std::make_pair(dx, dy);
        local = &grids[curGrid_];
    }

    void move(std::pair<int, int> dir)
    {
        cur_ += dir;
        auto [x, y] = cur_;
        if (0 <= x && x < SZ && 0 <= y && y < SZ)
        {
            return;
        }
        updateLocal();
    }
};

auto parse(const auto& lines, int infected = 1)
{
    auto h = static_cast<int>(lines.size());
    if (h == 0)
    {
        throw std::invalid_argument("Empty input");
    }
    auto w = static_cast<int>(lines.front().size());
    if (!std::ranges::all_of(lines, [w](auto& l) { return std::ssize(l) == w; }))
    {
        throw std::invalid_argument("Input not rectangular");
    }
    if (h % 2 != 1 || w % 2 != 1)
    {
        throw std::invalid_argument("Input has even size, can't find centre");
    }

    auto res = InfiniteGrid<GRID_SIZE>{};
    {
        using namespace std::views;
        for (auto [y, row] : zip(iota(0), lines))
        {
            for (auto [x, c] : zip(iota(0), row))
            {
                res[std::make_pair(x, y)] = (c == '#') ? infected : 0;
            }
        }
    }

    res.cur_ = std::make_pair(w / 2, h / 2);
    res.updateLocal();
    return res;
}

auto part1(const auto& lines)
{
    constexpr auto P1_STEPS = 10'000;

    auto grid = parse(lines);
    auto dir = std::make_pair(0, -1);
    auto res = 0;

    for (auto i = 0; i < P1_STEPS; ++i)
    {
        if (grid.cur())
        {
            dir = right(dir);
        }
        else
        {
            dir = left(dir);
            ++res;
        }
        grid.cur() ^= 1;
        grid.move(dir);
    }

    return res;
}

auto part2(const auto& lines)
{
    enum
    {
        CLEAN,
        WEAKENED,
        INFECTED,
        FLAGGED
    };
    constexpr auto STEPS = 10'000'000;

    auto grid = parse(lines, INFECTED);
    auto dir = std::make_pair(0, -1);
    auto res = 0;

    for (auto i = 0; i < STEPS; ++i)
    {
        switch (grid.cur())
        {
        case CLEAN:
            dir = left(dir);
            break;
        case WEAKENED:
            break;
        case INFECTED:
            dir = right(dir);
            break;
        case FLAGGED:
            dir = rev(dir);
            break;
        default:
            std::unreachable();
        }

        grid.cur() = (grid.cur() + 1) % 4;
        if (grid.cur() == INFECTED)
        {
            ++res;
        }
        grid.move(dir);
    }

    return res;
}

} // namespace
template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto lines = readAllLines(input);
    return {part1(lines), part2(lines)};
}
} // namespace aoc
