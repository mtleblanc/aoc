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
    size_t pc{};
    int steps{};
    for (; pc < offsets.size(); ++steps)
    {
        pc += offsets[pc]++;
    }
    return steps;
}

auto part2(std::vector<int> offsets)
{
    // loop is tight enough that using signed and comparing to 0 adds 50% runtime
    size_t pc{};
    int steps{};
    for (; pc < offsets.size(); ++steps)
    {
        auto& jmp = offsets[pc];
        pc += jmp;
        if (jmp >= 3)
        {
            --jmp;
        }
        else
        {
            ++jmp;
        }
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
