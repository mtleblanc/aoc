#include "aoc.hh"
#include <map>
#include <set>

/* https://adventofcode.com/2017/day/24
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 24;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
// TODO: Possible speedup by having Conn store a visited flag instead of having separate set<Conn*>
using Conn = std::pair<int, int>;

auto val(const Conn c)
{
    return c.first + c.second;
}

auto parse(auto& input)
{
    auto l = 0;
    auto r = 0;
    auto c = char{};
    auto res = std::vector<Conn>{};
    while (input >> l)
    {
        input >> c;
        input >> r;
        res.emplace_back(l, r);
    }
    return res;
}

auto listConnections(auto& components)
{
    auto res = std::map<int, std::vector<Conn*>>{};
    for (auto& c : components)
    {
        res[c.first].push_back(&c);
        res[c.second].push_back(&c);
    }
    return res;
}

auto dfs(const auto& connections, auto& seen, int currentNode = 0) -> int
{
    auto best = 0;
    if (!connections.contains(currentNode))
    {
        return best;
    }
    for (auto* c : connections.at(currentNode))
    {
        if (seen.contains(c))
        {
            continue;
        }
        seen.insert(c);
        auto otherPort = val(*c) - currentNode;
        auto cur = val(*c) + dfs(connections, seen, otherPort);
        best = std::max(best, cur);
        seen.erase(c);
    }
    return best;
}

auto part1(const auto& connections)
{
    auto seen = std::set<Conn*>{};
    return dfs(connections, seen);
}

auto dfs2(const auto& connections, auto& seen, int currentNode = 0,
          int depth = 0) -> std::pair<int, int>
{
    auto best = std::make_pair(depth, 0);
    if (!connections.contains(currentNode))
    {
        return best;
    }
    for (auto* c : connections.at(currentNode))
    {
        if (seen.contains(c))
        {
            continue;
        }
        seen.insert(c);
        auto otherPort = val(*c) - currentNode;
        auto [d, v] = dfs2(connections, seen, otherPort, depth + 1);
        auto cur = std::make_pair(d, v + val(*c));
        best = std::max(best, cur);
        seen.erase(c);
    }
    return best;
}

auto part2(const auto& connections)
{
    auto seen = std::set<Conn*>{};
    return dfs2(connections, seen).second;
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto components = parse(input);
    auto connections = listConnections(components);
    return {part1(connections), part2(connections)};
}
} // namespace aoc
