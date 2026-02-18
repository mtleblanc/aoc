#include "aoc.hh"
#include <algorithm>
#include <cassert>
#include <set>

/* https://adventofcode.com/2017/day/6
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 6;
using Solution = Solution_t<YEAR, DAY>;

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    constexpr auto BANKS = 16U;
    using Banks = std::array<int, BANKS>;

    auto blocks = readAll<int>(input);
    assert(blocks.size() == BANKS);
    Banks banks{};
    std::ranges::copy(blocks, banks.begin());

    std::set<Banks> seen;
    seen.insert(banks);
    std::optional<Banks> repeat;
    int part1{};
    for (int steps{1};;++steps)
    {
        // NOLINTNEXTLINE (readability-qualified-auto) iterator that happens to be a raw pointer
        auto max = std::ranges::max_element(banks);
        auto pebbles = *max;
        auto index = std::distance(banks.begin(), max);
        *max = 0;
        while (pebbles)
        {
            if (++index == BANKS)
            {
                index = 0;
            }
            --pebbles;
            ++banks[index];
        }
        if (repeat)
        {
            if (banks == repeat)
            {
                return {part1, steps - part1};
            }
        }
        else
        {
            if (seen.contains(banks))
            {
                part1 = steps;
                repeat = banks;
            }
            seen.insert(banks);
        }
    }
}
} // namespace aoc
