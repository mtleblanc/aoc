#include "aoc.hh"
#include <algorithm>
#include <map>
#include <queue>
#include <ranges>
#include <set>
#include <stdexcept>

/* https://adventofcode.com/2018/day/7
 */
namespace aoc
{
constexpr size_t YEAR = 2018;
constexpr size_t DAY = 7;

template <> struct SolutionType<YEAR, DAY>
{
    using type = StringSolution;
};
using Solution = Solution_t<YEAR, DAY>;

namespace
{
auto topSort(const auto& requirements)
{
    auto edges = std::map<char, std::vector<char>>{};
    auto prereqs = std::map<char, int>{};
    auto ready = std::set<char>{};
    for (const auto& [pre, post] : requirements)
    {
        if (++prereqs[post] == 1)
        {
            ready.erase(post);
        };
        if (prereqs[pre] == 0)
        {
            ready.insert(pre);
        }
        edges[pre].push_back(post);
    }
    auto res = std::string{};
    while (!ready.empty())
    {
        auto next = *ready.begin();
        res.push_back(next);
        ready.erase(next);
        for (auto post : edges[next])
        {
            if (--prereqs[post] == 0)
            {
                ready.insert(post);
            }
        }
    }
    if (std::ranges::any_of(prereqs, [](auto p) { return p.second > 0; }))
    {
        throw std::invalid_argument("Cyclic dependency");
    }
    return res;
}

template <int Workers, int Duration> auto parallelTopSort(const auto& requirements)
{
    auto edges = std::map<char, std::vector<char>>{};
    auto prereqs = std::map<char, int>{};
    auto ready = std::set<char>{};
    for (const auto& [pre, post] : requirements)
    {
        if (++prereqs[post] == 1)
        {
            ready.erase(post);
        };
        if (prereqs[pre] == 0)
        {
            ready.insert(pre);
        }
        edges[pre].push_back(post);
    }

    auto eventTimes = std::priority_queue<std::pair<int, char>, std::vector<std::pair<int, char>>,
                                          std::greater<>>{};
    auto readyWorkers = Workers;
    auto time = 0;
    while (!ready.empty() || !eventTimes.empty())
    {
        // if-elseif-else is important.  we need to finish ALL events at the current time before
        // assigning new workers, as they could ready events earlier in the alphabet.  Could also be
        // two while loops and be correct, but if the problem allowed instant task completion, we'd
        // have to go back to the eventTimes check after each assignment
        if (!eventTimes.empty() && eventTimes.top().first == time)
        {
            auto c = eventTimes.top().second;
            eventTimes.pop();
            ++readyWorkers;
            for (auto post : edges[c])
            {
                if (--prereqs[post] == 0)
                {
                    ready.insert(post);
                }
            }
        }
        else if (readyWorkers > 0 && !ready.empty())
        {
            --readyWorkers;
            auto c = *ready.begin();
            ready.erase(c);
            eventTimes.emplace(time + Duration + (c - 'A' + 1), c);
        }
        else
        {
            time = eventTimes.top().first;
        }
    }
    return time;
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    constexpr static auto PRE = 5;
    constexpr static auto POST = 36;
    constexpr static auto WORKERS = 5;
    constexpr static auto DURATION = 60;
    auto requirements = std::ranges::to<std::vector>(
        readAllLines(input) |
        std::views::transform([](const auto& l) { return std::make_pair(l[PRE], l[POST]); }));
    auto part1 = topSort(requirements);
    return {part1, std::to_string(parallelTopSort<WORKERS, DURATION>(requirements))};
}
} // namespace aoc
