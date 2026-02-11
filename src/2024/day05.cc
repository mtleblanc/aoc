#include "aoc.hh"
#include <algorithm>
#include <cassert>
#include <ios>
#include <iterator>
#include <map>
#include <ranges>
#include <set>
#include <sstream>
#include <utility>

/* https://adventofcode.com/2024/day/5
 *
 * Input:
 * 47|53
 * 97|13
 * ...
 *
 * 75,47,61,53,29
 * ...
 *
 * Part 1: The first section of input lists ordering requirements.  Find sequences in the
 * second section satisfy all requirements where both numbers appear and sum their middle numbers
 * Part 2: Sort the incorrect sequences and sum their middle numbers
 */
namespace aoc
{
constexpr size_t YEAR = 2024;
constexpr size_t DAY = 5;

namespace
{
struct OrderRule
{
    ssize_t first{};
    ssize_t second{};
    OrderRule(ssize_t first, ssize_t second) : first{first}, second{second} {};
    explicit OrderRule(const std::string& s);

    [[maybe_unused]] friend auto operator<=>(const OrderRule& lhs, const OrderRule& rhs) noexcept
    {
        return std::tie(lhs.first, lhs.second) <=> std::tie(rhs.first, rhs.second);
    }
};

std::istream& operator>>(std::istream& is, OrderRule& r)
{
    char c{};
    is >> r.first >> c >> r.second;
    if (c != '|')
    {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}
OrderRule::OrderRule(const std::string& s)
{
    std::istringstream iss{s};
    iss >> *this;
}

std::istream& operator>>(std::istream& is, std::vector<ssize_t>& seq)
{
    char c{};
    ssize_t t{};
    while (is >> t)
    {
        seq.push_back(t);
        if (is >> c && c != ',')
        {
            is.setstate(std::ios_base::failbit);
            return is;
        }
    }

    return is;
}

std::pair<bool, std::multimap<ssize_t, ssize_t>> inOrder(const std::vector<ssize_t>& seq,
                                             const std::set<OrderRule>& lu)
{
    std::multimap<ssize_t, ssize_t> applicableRules;
    bool result{true};
    for (auto oit = seq.begin(); oit != seq.end(); ++oit)
    {
        for (auto iit = seq.begin(); iit != oit; ++iit)
        {
            if (lu.contains({*oit, *iit}))
            {
                applicableRules.insert({*oit, *iit});
                result = false;
            }
            if (lu.contains({*iit, *oit}))
            {
                applicableRules.insert({*iit, *oit});
            }
        }
    }
    return {result, applicableRules};
}

void topSortHelper(const ssize_t n, std::multimap<ssize_t, ssize_t>& edges, auto& outIt,
                   std::set<ssize_t>& seen)
{
    for (auto [cur, end] = edges.equal_range(n); cur != end; ++cur)
    {
        if (!seen.contains(cur->second))
        {
            topSortHelper(cur->second, edges, outIt, seen);
        }
    }
    seen.insert(n);
    *outIt++ = n;
}

void topSort(const std::vector<ssize_t>& nodes, std::multimap<ssize_t, ssize_t>& edges, auto&& outIt)
{
    std::set<ssize_t> seen;
    for (auto n : nodes)
    {
        if (!seen.contains(n))
        {
            topSortHelper(n, edges, outIt, seen);
        }
    }
}

SsizeSolution solve(const std::vector<OrderRule>& rules,
               const std::vector<std::vector<ssize_t>>& sequences)
{
    SsizeSolution s;
    std::set<OrderRule> lu{rules.begin(), rules.end()};
    for (const auto& seq : sequences)
    {
        auto [isOrdered, rules] = inOrder(seq, lu);
        if (isOrdered)
        {
            s.part1 += seq[seq.size() / 2];
        }
        else
        {
            std::vector<ssize_t> sorted;
            sorted.reserve(seq.size());
            topSort(seq, rules, std::back_inserter(sorted));
            s.part2 += sorted[sorted.size() / 2];
        }
    }
    return s;
}

} // namespace

template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    auto lines = readAllLines(input);
    auto blank = std::ranges::find(lines, "");
    assert(blank != lines.end());
    std::vector<OrderRule> rules;
    for (auto it = lines.begin(); it != blank; ++it)
    {
        rules.emplace_back(*it);
    }
    std::vector<std::vector<ssize_t>> sequences;
    for (auto it = blank + 1; it != lines.end(); ++it)
    {
        std::istringstream iss{*it};
        sequences.emplace_back();
        iss >> sequences.back();
    }
    return solve(rules, sequences);
}
} // namespace aoc
