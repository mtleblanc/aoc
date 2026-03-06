#include "aoc.hh"
#include "util.hh"
#include <algorithm>
#include <limits>
#include <ranges>
#include <stdexcept>

/* https://adventofcode.com/2018/day/10
 */
namespace aoc
{
constexpr size_t YEAR = 2018;
constexpr size_t DAY = 10;

template <> struct SolutionType<YEAR, DAY>
{
    using type = StringSolution;
};
using Solution = Solution_t<YEAR, DAY>;

namespace
{
auto parse(const auto& line)
{
    auto numbers = readNumbers(line);
    if (numbers.size() != 4)
    {
        throw std::invalid_argument{"Malformed input"};
    }
    return std::make_pair(std::make_pair(numbers[0], numbers[1]),
                          std::make_pair(numbers[2], numbers[3]));
}

template <typename T> auto operator+(const std::pair<T, T>& l, const std::pair<T, T>& r)
{
    return std::make_pair(l.first + r.first, l.second + r.second);
}

template <typename T> auto operator-(const std::pair<T, T>& l, const std::pair<T, T>& r)
{
    return std::make_pair(l.first - r.first, l.second - r.second);
}

// abusing keys/values to get first and second components
auto width(const auto& particles)
{
    auto [min, max] = std::ranges::minmax(particles | std::views::keys | std::views::keys);
    return max - min;
}

auto display(const auto& particles)
{
    auto [minX, maxX] = std::ranges::minmax(particles | std::views::keys | std::views::keys);
    auto [minY, maxY] = std::ranges::minmax(particles | std::views::keys | std::views::values);
    // one extra width for \n
    auto w = maxX - minX + 2;
    auto h = maxY - minY + 1;
    // \n at beginning and end for output which expects inline
    auto res = std::string(w * h + 1, '.');
    for (auto y = 0; y <= h; ++y)
    {
        res[y * w] = '\n';
    }
    for (auto [x, y] :
         particles | std::views::transform([minX, minY](const auto& p)
                                           { return p.first - std::make_pair(minX, minY); }))
    {
        res[x + y * w + 1] = '#';
    }
    return res;
}

// copy so we can mutate
// particles start spread out and move towards each other.  the minimum spread will be at or near
// the time of the message, we'll just guess it's exactly the right time, and adjust if needed
// to detect minimum, we just wait for the size to go up, then go back one step.  We could maintain
// two sets of particles and avoid the reversing but it's unneeded
auto simulate(auto particles)
{
    auto lastWidth = std::numeric_limits<int>::max();
    auto newWidth = 0;
    auto steps = 0;
    while ((newWidth = width(particles)) < lastWidth)
    {
        lastWidth = newWidth;
        ++steps;
        std::ranges::for_each(particles, [](auto& p) { p.first = p.first + p.second; });
    }
    --steps;
    std::ranges::for_each(particles, [](auto& p) { p.first = p.first - p.second; });
    return Solution{display(particles), std::to_string(steps)};
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto particles = std::ranges::to<std::vector>(
        readAllLines(input) | std::views::transform([](const auto& l) { return parse(l); }));
    return simulate(particles);
}
} // namespace aoc
