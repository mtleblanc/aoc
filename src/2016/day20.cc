#include "aoc.hh"
#include <algorithm>
#include <ranges>

/* https://adventofcode.com/2016/day/20
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 20;

namespace
{
struct Range
{
    int64_t low;
    int64_t high;
};

[[maybe_unused]] std::istream& operator>>(std::istream& is, Range& r)
{
    char c{};
    is >> r.low >> c >> r.high;
    if (c != '-')
    {
        is.setstate(std::ios_base::failbit);
    }
    if (r.low > r.high)
    {
        throw std::invalid_argument("Blacklist range has low > high");
    }
    return is;
}

GeneralSolution<int64_t> processBlacklist(const std::vector<Range>& ranges)
{
    std::vector<std::pair<int64_t, int>> endpoints;
    endpoints.reserve(ranges.size() * 2);
    std::ranges::copy(ranges |
                          std::views::transform(
                              [](auto r)
                              {
                                  return std::array<std::pair<int64_t, int>, 2>{
                                      {std::make_pair(r.low, -1), std::make_pair(r.high, 1)}};
                              }) |
                          std::views::join,
                      std::back_inserter(endpoints));
    std::ranges::make_heap(endpoints, std::greater<>());
    std::optional<int64_t> lowest;
    int64_t whitelistCount{};
    // my input has 0 and uint32::max both blacklisted, so this and the end count aren't necessary,
    // but are kept in for correctness
    if (endpoints.front().first != 0)
    {
        lowest = 0;
        whitelistCount += endpoints.front().first;
    }
    int nesting{};
    // stop 1 early, it's guaranteed to be the highest blacklisted number and we want it available
    // outside the loop to count IPs up to uint32::max.  Not required for my input, but kept for
    // correctness
    while (endpoints.size() > 1)
    {
        auto next = endpoints.front();
        std::ranges::pop_heap(endpoints, std::greater<>());
        endpoints.pop_back();
        nesting += next.second;
        // endpoints are ordered so the opening of a block is before a closing of another block at
        // the same IP.  So in those cases, we won't hit nesting == 0.  This
        // saves a check that blockSize is non-negative.  It also guarantees that at nesting == 0,
        // the next endpoint is a opening, which, though not required, makes it easier to reason
        // about
        if (nesting == 0)
        {
            auto blockStart = next.first + 1;
            auto blockSize = endpoints.front().first - blockStart;
            if (blockSize && !lowest)
            {
                lowest = blockStart;
            }
            whitelistCount += blockSize;
        }
    }
    whitelistCount += std::numeric_limits<uint32_t>::max() - endpoints.front().first;
    return {lowest.value_or(-1), whitelistCount};
}
} // namespace

template <> struct SolutionType<YEAR, DAY>
{
    using type = GeneralSolution<int64_t>;
};

template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    auto ranges = readAll<Range>(input);
    return processBlacklist(ranges);
}
} // namespace aoc
