#include "aoc.hh"
#include <cassert>
#include <string>
#include <utility>
namespace aoc
{
constexpr size_t YEAR = 2025;
constexpr size_t DAY = 3;
namespace
{
template <int N> size_t solveFor(const std::vector<std::string>& lines)
{
    constexpr uint64_t BASE = 10;
    uint64_t accum{};
    std::array<char, N> maxes{};
    for (const auto& s : lines)
    {
        if (s.length() < N)
        {
            break;
        }
        // just start at the end and move backwards greedily
        // if maxes were pointers and maxes[-1] were the iterator,
        // we'd have an invariant that maxes[i] points to the leftmost largest element both right of
        // maxes[i-1] and at least N-i from the end but we only need the values, as long as we don't
        // break on equality (eg in xxxxx76, when moving to xxxx776 we have to move maxes[0] to the
        // first 7 to make space for maxes[1] on the second 7)
        auto rit = s.crbegin();
        for (int i = N - 1; i >= 0; i--)
        {
            maxes[i] = *(rit++);
        }
        for (; rit != s.crend(); ++rit)
        {
            char c{*rit};
            for (int i = 0; i < N; i++)
            {
                if (c < maxes[i])
                {
                    break;
                }
                std::swap(c, maxes[i]);
            }
        }
        uint64_t best{0};
        for (int i = 0; i < N; i++)
        {
            best *= BASE;
            best += maxes[i] - '0';
        }
        accum += best;
    }
    return accum;
}
} // namespace
template <> Solution solve<YEAR, DAY>(const std::vector<std::string>& lines)
{
    constexpr int PART1_N = 2;
    constexpr int PART2_N = 12;
    return Solution{solveFor<PART1_N>(lines), solveFor<PART2_N>(lines)};
}
} // namespace aoc