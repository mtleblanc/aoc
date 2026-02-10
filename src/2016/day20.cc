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

// Half-open range
struct Range
{
    int64_t low;
    int64_t high;
};

struct EP
{
    int64_t val;
    int nest;

    auto operator<=>(const EP& o) const = default;
};

[[maybe_unused]] std::istream& operator>>(std::istream& is, Range& r)
{
    char c{};
    is >> r.low >> c >> r.high;
    // change from endpoint inclusive to half-open
    ++r.high;
    if (!is || c != '-' || r.low > r.high)
    {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}

GeneralSolution<int64_t> processBlacklist(const std::vector<Range>& ranges)
{
    // NB - range converted to [low, high)
    std::vector<EP> endpoints;
    endpoints.reserve(ranges.size() * 2);
    std::ranges::copy(
        ranges |
            std::views::transform([](auto r) { return std::array{EP{r.low, -1}, EP{r.high, 1}}; }) |
            std::views::join,
        std::back_inserter(endpoints));
    std::ranges::sort(endpoints);
    std::optional<int64_t> lowest;
    int64_t whitelistCount{};
    // my input has 0 and uint32::max both blacklisted, so this isn't necessary,
    // but is kept in for correctness
    if (endpoints.front().val != 0)
    {
        lowest = 0;
    }
    whitelistCount += endpoints.front().val;
    whitelistCount += std::numeric_limits<uint32_t>::max() + 1L - endpoints.back().val;
    int nesting{};
    for (auto [cur, next] : std::views::zip(endpoints, endpoints | std::views::drop(1)))
    {
        // NB ordering of half open endpoints has opening of new block before closing of previous.
        // Along with input validation that high > low, this guarantees that when nesting == 0, we
        // have cur is a closing, next an opening, and next.first > cur.first
        nesting += cur.nest;
        if (nesting == 0)
        {
            if (!lowest)
            {
                lowest = cur.val;
            }
            whitelistCount += next.val - cur.val;
        }
    }
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
