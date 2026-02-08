#include "aoc.hh"
#include <algorithm>
#include <functional>
#include <queue>
#include <ranges>
#include <set>

/* https://adventofcode.com/2016/day/11
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 11;

namespace
{
constexpr auto FLOORS = 4UL;
struct RTG
{
    bool isChip;
    size_t type;
    size_t floor;

    [[nodiscard]] bool shieldedBy(const RTG& o) const
    {
        return o.type == type && o.isChip == !isChip && o.floor == floor;
    }
    [[nodiscard]] bool friedBy(const RTG& o) const
    {
        return isChip && !o.isChip && o.type != type && o.floor == floor;
    }
    [[nodiscard]] auto operator<=>(const RTG& o) const
    {
        return std::tie(type, isChip) <=> std::tie(o.type, o.isChip);
    }
};

struct State
{
    std::vector<RTG> rtgs;
    size_t elevator{};
    size_t steps{};

    [[nodiscard]] std::vector<size_t> sig() const
    {
        auto sg = rtgs | std::views::transform([](const auto& r) { return r.floor; }) |
                  std::ranges::to<std::vector>();
        sg.push_back(elevator);
        return sg;
    }

    [[nodiscard]] size_t astarDistance() const
    {
        auto costs =
            rtgs | std::views::transform([](const auto& rtg) { return 2 * (FLOORS - rtg.floor); });
        return steps + std::ranges::fold_left(costs, 0UL, std::plus<>()) - (FLOORS - elevator) * 3;
    }

    [[nodiscard]] bool isValid() const
    {
        // floors are 1-based
        if (0 == elevator || elevator > FLOORS)
        {
            return false;
        }
        auto shielded = rtgs |
                        std::views::filter(
                            [this](const auto& rtg)
                            {
                                return std::ranges::any_of(this->rtgs, [&rtg](const auto& r)
                                                           { return rtg.shieldedBy(r); });
                            }) |
                        std::ranges::to<std::set>();
        auto fried = rtgs | std::views::filter(
                                [this](const auto& rtg)
                                {
                                    return std::ranges::any_of(this->rtgs, [&rtg](const auto& r)
                                                               { return rtg.friedBy(r); });
                                });
        return std::ranges::all_of(fried,
                                   [&shielded](const auto& rtg) { return shielded.contains(rtg); });
    }

    [[nodiscard]] bool isDone() const
    {
        return std::ranges::all_of(rtgs, [](const auto& rtg) { return rtg.floor == FLOORS; });
    }

    [[nodiscard]] std::vector<State> nextStates() const
    {
        std::vector<State> validNextStates;

        auto newState = *this;
        ++newState.steps;
        for (auto& rtg :
             newState.rtgs | std::views::filter([this](const auto& rtg)
                                                { return rtg.floor == this->elevator; }))
        {
            if (elevator > 1)
            {
                newState.elevator = --rtg.floor;
                if (newState.isValid())
                {
                    validNextStates.push_back(newState);
                }
                for (auto& rtg2 :
                     newState.rtgs |
                         std::views::filter([this, &rtg](const auto& rtg2)
                                            { return rtg2 > rtg && rtg2.floor == this->elevator; }))
                {
                    --rtg2.floor;
                    if (newState.isValid())
                    {
                        validNextStates.push_back(newState);
                    }
                    ++rtg2.floor;
                }
                ++rtg.floor;
            }
            if (elevator < FLOORS)
            {
                newState.elevator = ++rtg.floor;
                if (newState.isValid())
                {
                    validNextStates.push_back(newState);
                }
                for (auto& rtg2 :
                     newState.rtgs |
                         std::views::filter([this, &rtg](const auto& rtg2)
                                            { return rtg2 > rtg && rtg2.floor == this->elevator; }))
                {
                    ++rtg2.floor;
                    if (newState.isValid())
                    {
                        validNextStates.push_back(newState);
                    }
                    --rtg2.floor;
                }
                --rtg.floor;
            }
        }
        return validNextStates;
    }

    auto operator<=>(const State& o) const
    {
        return astarDistance() <=> o.astarDistance();
    }
};

size_t solve(std::vector<RTG> start)
{
    std::set<std::vector<size_t>> seen;
    State s{};
    s.elevator = 1;
    s.steps = 0;
    s.rtgs = std::move(start);
    std::priority_queue<State, std::vector<State>, std::greater<>> frontier;
    seen.insert(s.sig());
    frontier.push((s));
    while (frontier.size())
    {
        State s = frontier.top();
        frontier.pop();
        if (s.isDone())
        {
            return s.steps;
        }
        for (auto& ns : s.nextStates())
        {
            if (!seen.contains(ns.sig()))
            {
                frontier.push(std::move(ns));
                seen.insert(ns.sig());
            }
        }
    }
    return 0;
}

} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    (void)input;

    std::vector<RTG> s = {{true, 0, 1}, {false, 0, 1}, {false, 1, 1}, {false, 2, 1}, {true, 1, 2},
                          {true, 2, 2}, {true, 3, 3},  {false, 3, 3}, {true, 4, 3},  {false, 4, 3}};

    auto part1 = solve(s);
    // s.emplace_back(true, 5, 1);
    // // s.emplace_back(true, 6, 1);
    // s.emplace_back(false, 5, 1);
    // s.emplace_back(false, 6, 1);
    return {part1, solve(s)};
}
} // namespace aoc
