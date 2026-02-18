#include "aoc.hh"

/* https://adventofcode.com/2017/day/5
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 5;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
auto part1(std::vector<int> offsets)
{
    int pc{};
    int steps{};
    while (0 <= pc && pc < std::ssize(offsets))
    {
        pc += offsets[pc]++;
        ++steps;
    }
    return steps;
}

auto part2(std::vector<int> offsets)
{
    int pc{};
    int steps{};
    while (0 <= pc && pc < std::ssize(offsets))
    {
        auto offset = offsets[pc];
        if (offset >= 3)
        {
            --offsets[pc];
        }
        else
        {
            ++offsets[pc];
        }
        pc += offset;
        ++steps;
    }
    return steps;
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto offsets = readAll<int>(input);
    return {part1(offsets), part2(offsets)};
}
} // namespace aoc
