#include "aoc.hh"
#include <algorithm>
#include <map>
#include <ranges>

/* https://adventofcode.com/2018/day/2
 */
namespace aoc
{
constexpr size_t YEAR = 2018;
constexpr size_t DAY = 2;

template <> struct SolutionType<YEAR, DAY>
{
    using type = StringSolution;
};
using Solution = Solution_t<YEAR, DAY>;

namespace
{
auto countLetters(std::string_view s)
{
    auto res = std::map<char, int>{};
    for (auto c : s)
    {
        ++res[c];
    }
    return res;
}

auto part1(const auto& ids)
{
    auto containsExactly = [](auto n)
    {
        return [n](const auto& counts)
        { return std::ranges::contains(counts | std::views::values, n); };
    };
    auto letterCounts = ids | std::views::transform(countLetters);
    return std::ranges::count_if(letterCounts, containsExactly(2)) *
           std::ranges::count_if(letterCounts, containsExactly(3));
}

auto matchingChars(const auto& l, const auto& r)
{
    using namespace std::views;
    auto v = zip(l, r) |
             filter(
                 [](auto p)
                 {
                     auto [l, r] = p;
                     return l == r;
                 }) |
             std::views::keys;
    return std::string{v.begin(), v.end()};
}

auto part2(const auto& ids)
{
    // input is only 250, fine to have duplicate comparisons.  The equal size test isn't needed on
    // the input but is kept for correctness
    using namespace std::views;
    auto v = cartesian_product(ids, ids) |
             transform(
                 [](auto p) -> std::optional<std::string>
                 {
                     auto [l, r] = p;
                     if (l.size() != r.size())
                     {
                         return {};
                     }
                     auto res = matchingChars(l, r);
                     return res.size() + 1 == l.size() ? std::optional{res} : std::nullopt;
                 }) |
             filter([](auto p) { return p.has_value(); });
    auto it = v.begin();
    return it == v.end() ? std::string{} : **it;
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto ids = readAllLines(input);
    return {std::to_string(part1(ids)), part2(ids)};
}
} // namespace aoc
