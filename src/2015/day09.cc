#include "aoc.hh"
#include <algorithm>
#include <istream>
#include <map>
#include <ranges>
#include <set>

/* https://adventofcode.com/2015/day/9
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 9;

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
    is >> d.from >> tmp >> d.to >> tmp >> d.dist;
    return is;
}

long tsp(const std::vector<std::vector<long>>& distanceMatrix, auto reducer)
{
    auto calculateDistance = [&](const std::vector<size_t>& path)
    {
        long total = 0;
        for (size_t i = 0; i < path.size() - 1; ++i)
        {
            total += distanceMatrix[path[i]][path[i + 1]];
        }
        return total;
    };

    auto order = std::ranges::to<std::vector>(std::views::iota(0UL, distanceMatrix.size()));

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
        dist[fi][ti] = dist[ti][fi] = distance;
    }

    return {tsp(dist, [](auto r) { return std::ranges::min(r); }),
            tsp(dist, [](auto r) { return std::ranges::max(r); })};
}
} // namespace aoc
