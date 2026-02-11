#include "aoc.hh"
#include <algorithm>
#include <map>
#include <numeric>

/* https://adventofcode.com/2024/day/1
 *
 * Input:
 * 1 2
 * 3 4
 * ...
 *
 * Part 1: find the sum of differences between the columns after they are sorted
 *
 * Part 2: find the sum of each number in left list multiplied by the number of occurrences of said
 * number in the right list
 */
namespace aoc
{
constexpr size_t YEAR = 2024;
constexpr size_t DAY = 1;

template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    int l{};
    int r{};
    std::vector<int> left;
    std::vector<int> right;
    while (input >> l >> r)
    {
        left.push_back(l);
        right.push_back(r);
    }
    std::ranges::sort(left);
    std::ranges::sort(right);
    SsizeSolution s;
    s.part1 = std::transform_reduce(left.begin(), left.end(), right.begin(), 0, std::plus<>(),
                                    [](auto l, auto r) { return std::abs(l - r); });

    // since both lists are sorted, we could easily count occurrences by just iterating through both
    // lists in parallel but it's less code to just use a map
    std::map<int, int> oc;
    std::ranges::for_each(right, [&oc](auto r) { oc[r]++; });
    s.part2 = std::transform_reduce(left.begin(), left.end(), 0, std::plus<>(),
                                    [&oc](auto l) { return oc[l] * l; });
    return s;
}
} // namespace aoc