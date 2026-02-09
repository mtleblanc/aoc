#include "aoc.hh"
#include <algorithm>
#include <optional>
#include <span>

/* https://adventofcode.com/2015/day/24
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 24;

namespace
{

/* Uses dp to solve packing, the array at the start of each loop contains the best solutions for
 * each target using only the first n sizes, with optional{} indicating no solution
 */
ssize_t pack(std::span<ssize_t> vals, ssize_t target)
{
    // first index: number of items included
    // second index: total size of items included
    // value: smallest product of sizes
    std::vector<std::vector<std::optional<ssize_t>>> dp;
    std::optional<ssize_t> smallestGroupFound;
    for (auto v : vals)
    {
        if (v > target)
        {
            continue;
        }
        if (!smallestGroupFound)
        {
            dp.emplace_back(target + 1);
        }
        // need to scan backwards so that don't use the same value twice, no need to scan the empty
        // row just added, or any rows including and beyone the smallest row with a solution
        for (ssize_t i{smallestGroupFound.value_or(dp.size() - 1)}; i-- > 0;)
        {
            // since v <= target and dp[i].size() == target + 1, these indices are all valid and no
            // check vs dp[i].size() is needed
            for (ssize_t prevSum{0}; prevSum + v <= target; ++prevSum)
            {
                if (dp[i][prevSum])
                {
                    auto& next = dp[i + 1][prevSum + v];
                    auto nextVal = *dp[i][prevSum] * v;
                    next = std::min(next.value_or(nextVal), nextVal);
                    if (prevSum + v == target)
                    {
                        smallestGroupFound = std::min(smallestGroupFound.value_or(i + 1), i + 1);
                    }
                }
            }
        }
        // base case
        dp[0][v] = v;
    }
    if (!smallestGroupFound)
    {
        return 0;
    }
    return *dp[*smallestGroupFound].back();
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto boxes = readAll<ssize_t>(input);
    auto total = std::ranges::fold_left(boxes, 0, std::plus<>());
    return {pack(boxes, total / 3), pack(boxes, total / 4)};
}
} // namespace aoc
