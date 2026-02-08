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
using Coord = std::pair<ssize_t, ssize_t>;
struct Path
{
    Coord c;
    size_t steps;
};

bool valid(Coord coords, size_t seed)
{
    auto [x, y] = coords;
    return x >= 0 && y >= 0 && std::popcount(x * x + 3 * x + 2 * x * y + y + y * y + seed) % 2 == 0;
}

Coord operator+(const Coord& l, const Coord& r)
{
    return {l.first + r.first, l.second + r.second};
}

constexpr auto DIRS = std::array<Coord, 4>{{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};

size_t part1(size_t seed)
{
    constexpr auto TARGET = std::make_pair(31UL, 39UL);
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

size_t part2(size_t seed)
{
    constexpr auto MAX_STEPS = 50UL;
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
    return seen.size();
}
} // namespace
template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    size_t seed{};
    input >> seed;

    return {part1(seed), part2(seed)};
}
} // namespace aoc
