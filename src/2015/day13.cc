#include "aoc.hh"
#include <algorithm>
#include <istream>
#include <map>
#include <ranges>
#include <set>

/* https://adventofcode.com/2015/day/13
 *
 * Basically day 9.  Just need to add the benefits/costs in both directions during setup, and
 * account for the wraparound in part 1.  part 2 is identical to day9 part 2, adding a 0 cost node
 * is the same as not wrapping around
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 13;

namespace
{
struct Distance
{
    std::string from;
    std::string to;
    long dist{};
};

std::istream& operator>>(std::istream& is, Distance& d)
{
    std::string tmp;
    ;
    std::string gainLose;
    // Alice would lose 2 happiness units by sitting next to Bob.
    is >> d.from >> tmp >> gainLose >> d.dist >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> d.to;

    if (gainLose == "lose")
    {
        d.dist = -d.dist;
    }

    // remove period
    d.to.pop_back();
    return is;
}

// A bit of overcleverness, when wrapping around, rotational symmetry lets us consider 1/nth the
// cases so we just fix the last person at the end and only order the first n-1.  The nth person has
// index n-1 == path.size().  We could also try to reduce the cases by 1/2 by reverse symmetry, but
// I don't have an easy way to iterate through that half.  We could consider only (n-1)/(n-2) by
// insisting that (say) n-1 is never first, but that's minimal speedup
template <bool WRAP> long tsp(const std::vector<std::vector<long>>& distanceMatrix, auto reducer)
{
    // implicit path.size() at end of path to deal with rotational symmetry
    auto calculateDistance = [&](const std::vector<size_t>& path)
    {
        long total = 0;
        for (size_t i = 0; i < path.size() - 1; ++i)
        {
            total += distanceMatrix[path[i]][path[i + 1]];
        }
        if constexpr (WRAP)
        {
            total += distanceMatrix[path.back()][path.size()];
            total += distanceMatrix[path.size()][path.front()];
        }
        return total;
    };

    auto order =
        std::ranges::to<std::vector>(std::views::iota(0UL, distanceMatrix.size() - (WRAP ? 1 : 0)));

    std::vector<long> routeDistances;
    do
    {
        routeDistances.push_back(calculateDistance(order));
    } while (std::ranges::next_permutation(order).found);

    return reducer(routeDistances);
}

} // namespace
template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    std::vector<Distance> edges;
    for (Distance d; input >> d;)
    {
        edges.emplace_back(d);
    }

    std::set<std::string> names;
    for (const auto& [from, to, _] : edges)
    {
        names.insert(from);
        names.insert(to);
    }

    std::map<std::string, size_t> idxs;
    for (size_t idx{}; const auto& city : names)
    {
        idxs[city] = idx++;
    }

    auto n = names.size();
    std::vector<std::vector<long>> dist(n, std::vector<long>(n));
    for (const auto& [from, to, distance] : edges)
    {
        auto fi = idxs[from];
        auto ti = idxs[to];
        dist[fi][ti] += distance;
        dist[ti][fi] += distance;
    }

    return {tsp<true>(dist, [](auto r) { return std::ranges::max(r); }),
            tsp<false>(dist, [](auto r) { return std::ranges::max(r); })};
}
} // namespace aoc
