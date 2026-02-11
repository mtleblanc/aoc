#include "aoc.hh"
#include <span>

/* https://adventofcode.com/2015/day/17
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 17;

namespace
{

ssize_t part1(std::span<ssize_t> containers, ssize_t target)
{
    if (containers.empty())
    {
        return target == 0 ? 1 : 0;
    }
    auto next = containers.subspan(1);
    return part1(next, target) +
           (containers.front() > target ? 0 : part1(next, target - containers.front()));
}

// We could do this in one pass and count how many containers are used for each match, but input
// size is small enough that just trying n=1,2,.. runs in 3.9ms
ssize_t part2(std::span<ssize_t> containers, ssize_t target, ssize_t n)
{
    if (containers.empty() || n == 0)
    {
        return target == 0 ? 1 : 0;
    }
    auto next = containers.subspan(1);
    return part2(next, target, n) +
           (containers.front() > target ? 0 : part2(next, target - containers.front(), n - 1));
}
} // namespace
template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    constexpr ssize_t TARGET_SIZE = 150;
    std::vector<ssize_t> containers;
    for (ssize_t sz{}; input >> sz;)
    {
        containers.push_back(sz);
    }

    ssize_t p2{};
    for (ssize_t n{1}; p2 == 0; ++n)
    {
        p2 = part2(containers, TARGET_SIZE, n);
    }

    return {part1(containers, TARGET_SIZE), p2};
}
} // namespace aoc
