#include "aoc.hh"
#include <deque>
#include <set>

/* https://adventofcode.com/2016/day/13
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 13;

namespace
{
using Coord = std::pair<int, int>;
struct Path
{
    Coord c;
    int steps;
};

bool valid(Coord coords, int seed)
{
    auto [x, y] = coords;
    return x >= 0 && y >= 0 && std::popcount(static_cast<uint32_t>(x * x + 3 * x + 2 * x * y + y + y * y + seed)) % 2 == 0;
}

Coord operator+(const Coord& l, const Coord& r)
{
    return {l.first + r.first, l.second + r.second};
}

constexpr auto DIRS = std::array<Coord, 4>{{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};

int part1(int seed)
{
    constexpr auto TARGET = std::make_pair(31, 39);
    std::set<Coord> seen;
    std::deque<Path> frontier;
    Coord start = {1, 1};
    seen.insert(start);
    frontier.push_back({start, 0});
    while (frontier.size())
    {
        auto [c, s] = frontier.front();
        frontier.pop_front();
        ++s;
        for (auto d : DIRS)
        {
            auto n = c + d;
            if (n == TARGET)
            {
                return s;
            }
            if (!valid(n, seed) || seen.contains(n))
            {
                continue;
            }
            seen.insert(n);
            frontier.emplace_back(n, s);
        }
    }
    // unreachable
    return 0;
}

int part2(int seed)
{
    constexpr auto MAX_STEPS = 50;
    std::set<Coord> seen;
    std::deque<Path> frontier;
    Coord start = {1, 1};
    seen.insert(start);
    frontier.push_back({start, 0});
    while (frontier.size())
    {
        auto [c, s] = frontier.front();
        frontier.pop_front();
        if (s == MAX_STEPS)
        {
            continue;
        }
        ++s;
        for (auto d : DIRS)
        {
            auto n = c + d;
            if (!valid(n, seed) || seen.contains(n))
            {
                continue;
            }
            seen.insert(n);
            if (s < MAX_STEPS)
            {
                frontier.emplace_back(n, s);
            }
        }
    }
    return static_cast<int>(seen.size());
}
} // namespace
template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    int seed{};
    input >> seed;

    return {part1(seed), part2(seed)};
}
} // namespace aoc
