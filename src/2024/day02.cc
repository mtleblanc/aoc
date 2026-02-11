#include "aoc.hh"
#include <algorithm>
#include <sstream>

/* https://adventofcode.com/2024/day/2
 *
 * Input:
 * 7 6 4 2 1
 * 1 2 7 8 9
 * ...
 *
 * Part 1: Count how many rows are monotone with differences between 1 and 3 inclusive
 * Part 2: Count how many rows could satisfy part 1 if a single value were removed
 */
namespace aoc
{
constexpr size_t YEAR = 2024;
constexpr size_t DAY = 2;

namespace
{
inline std::vector<int> toVec(const std::string& s)
{
    std::istringstream iss{s};
    std::vector<int> res;
    int t{};
    while (iss >> t)
    {
        res.push_back(t);
    }
    return res;
}

inline bool isGradual(const std::vector<int>& v)
{
    if (v.size() < 2)
    {
        return true;
    }
    int s = v[0] < v[1] ? 1 : -1;
    auto cmp = [s](auto l, auto r)
    {
        int d = (r - l) * s;
        return d < 1 || d > 3;
    };
    return std::ranges::adjacent_find(v, cmp) == v.end();
}

inline bool isSemiGradual(const std::vector<int>& v)
{
    if (v.size() < 3)
    {
        return true;
    }
    // Not going to bother trying to 'fix' a failing sequence, no input is long, we'll just try
    // deleting each element. Start from the end and swap the removed element back in moving
    // backwards
    //
    // 1 2 3 (4 removed)
    // swap 4 and 3
    // 1 2 4 (3 removed)
    // swap 3 and 2
    // 1 3 4 (2 removed)
    // swap 2 and 1
    // 2 3 4
    //
    // Note we need one more check than swaps (before first and after last)
    std::vector<int> d{v};
    int removed = d.back();
    d.pop_back();
    for (size_t i{d.size()}; i > 0; i--)
    {
        if (isGradual(d))
        {
            return true;
        }
        std::swap(removed, d[i - 1]);
    }
    return isGradual(d);
}

} // namespace

template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    auto rss = readAllLines(input);
    std::vector<std::vector<int>> rsi;
    std::ranges::transform(rss, back_inserter(rsi), toVec);
    return {std::ranges::count_if(rsi, isGradual),
            std::ranges::count_if(rsi, isSemiGradual)};
}
} // namespace aoc