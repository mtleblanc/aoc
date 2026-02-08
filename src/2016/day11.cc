#include "aoc.hh"
#include <algorithm>
#include <cassert>
#include <functional>
#include <queue>
#include <ranges>
#include <set>
#include <utility>

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
    using Sig = std::vector<size_t>;
    std::vector<RTG> rtgs;
    size_t elevator{};
    size_t steps{};

    [[nodiscard]] Sig sig() const
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

template <size_t N> struct ArrayState
{
    using Rep = std::array<uint8_t, 2 * N + 1>;
    // generator i at floors[2*i], chip at floors[2*i + 1], elevator at floors[2*N]
    Rep floors;
    size_t steps{};

    [[nodiscard]] constexpr uint8_t& elevator()
    {
        return floors.back();
    }

    [[nodiscard]] constexpr const uint8_t& elevator() const
    {
        return floors.back();
    }

    [[nodiscard]] constexpr uint8_t& chip(size_t i)
    {
        return floors[2 * i + 1];
    }
    [[nodiscard]] constexpr const uint8_t& chip(size_t i) const
    {
        return floors[2 * i + 1];
    }
    [[nodiscard]] constexpr uint8_t& gen(size_t i)
    {
        return floors[2 * i];
    }
    [[nodiscard]] constexpr const uint8_t& gen(size_t i) const
    {
        return floors[2 * i];
    }

    [[nodiscard]] constexpr size_t astarDistance() const
    {
        // To find min moves, consider one item permanently in the elevator, and each other item
        // only moves up. Then each item starting on floor f accounts for FLOORS - f up-moves.  The
        // net movement of the elevator is FLOORS - elevator, so factoring that out to subtract
        // later, each items up move also generates a down move for net 0
        auto elevatorNet = FLOORS - elevator();
        auto chipNGenMoves = FLOORS * (4 * N) -
                             2 * (std::ranges::fold_left(floors, 0UL, std::plus<>()) - elevator());
        // but we are carrying one element permanently and overcounted it, which can be any item
        // from the elevator's starting floor, i.e. another elevatorNet*2
        // there's an edgecase if only one item is on the elevator's floor and no items are below
        // it, this can count negative, so clamp
        return chipNGenMoves - std::min(3 * elevatorNet, chipNGenMoves) + steps;
    }

    constexpr auto operator<=>(const ArrayState<N>& o) const
    {
        return astarDistance() <=> o.astarDistance();
    }

    [[nodiscard]] constexpr bool isDone() const
    {
        return std::ranges::all_of(floors, [](auto f) { return f == FLOORS; });
    }

    [[nodiscard]] constexpr bool isValid() const
    {
        for (size_t i{}; i < N; ++i)
        {
            if (chip(i) == gen(i))
            {
                continue;
            }
            for (size_t j{}; j < N; ++j)
            {
                if (chip(i) == gen(j))
                {
                    return false;
                }
            }
        }
        return true;
    }

    [[nodiscard]] std::vector<ArrayState<N>> nextStates() const
    {
        std::vector<ArrayState<N>> nexts;
        for (size_t idx{}; idx < floors.size() - 1; ++idx)
        {
            if (floors[idx] != elevator())
            {
                continue;
            }
            if (elevator() > 1)
            {
                auto next = *this;
                ++next.steps;
                --next.floors[idx];
                --next.elevator();
                if (next.isValid())
                {
                    nexts.push_back(next);
                }
                for (size_t idx2{idx + 1}; idx2 < floors.size() - 1; ++idx2)
                {
                    if (floors[idx2] != elevator())
                    {
                        continue;
                    }
                    auto next2 = next;
                    --next2.floors[idx2];
                    if (next2.isValid())
                    {
                        nexts.push_back(next2);
                    }
                }
            }
            if (elevator() < FLOORS)
            {
                auto next = *this;
                ++next.steps;
                ++next.floors[idx];
                ++next.elevator();
                if (next.isValid())
                {
                    nexts.push_back(next);
                }
                for (size_t idx2{idx + 1}; idx2 < floors.size() - 1; ++idx2)
                {
                    if (floors[idx2] != elevator())
                    {
                        continue;
                    }
                    auto next2 = next;
                    ++next2.floors[idx2];
                    if (next2.isValid())
                    {
                        nexts.push_back(next2);
                    }
                }
            }
        }
        return nexts;
    }

    using Sig = Rep;
    [[nodiscard]] auto sig() const
    {
        return floors;
    }
};

template <typename S> size_t solve(S s)
{
    std::set<typename S::Sig> seen;
    std::priority_queue<S, std::vector<S>, std::greater<>> frontier;
    seen.insert(s.sig());
    frontier.push((s));
    while (frontier.size())
    {
        S s = frontier.top();
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
    State start{};
    start.elevator = 1;
    start.steps = 0;
    start.rtgs = std::move(s);
    // auto part1 = solve<State>(start);
    auto part1 = solve<ArrayState<4>>({.floors = {1, 1, 1, 2, 1, 2, 3, 3,1}});
    // s.emplace_back(true, 5, 1);
    // // s.emplace_back(true, 6, 1);
    // s.emplace_back(false, 5, 1);
    // s.emplace_back(false, 6, 1);
    return {part1, /*solve<State>(start)*/ 0};
}
} // namespace aoc
