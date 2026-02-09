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
constexpr auto FLOORS = 4;

template <int N> struct ArrayState
{
    using Item = uint8_t;
    using Rep = std::array<Item, 2 * N + 1>;
    // generator i at floors[2*i], chip at floors[2*i + 1], elevator at floors[2*N]
    Rep floors;
    int steps{};

    [[nodiscard]] constexpr Item& elevator()
    {
        return floors.back();
    }

    [[nodiscard]] constexpr const Item& elevator() const
    {
        return floors.back();
    }

    [[nodiscard]] constexpr Item& chip(int i)
    {
        return floors[2 * i + 1];
    }
    [[nodiscard]] constexpr const Item& chip(int i) const
    {
        return floors[2 * i + 1];
    }
    [[nodiscard]] constexpr Item& gen(int i)
    {
        return floors[2 * i];
    }
    [[nodiscard]] constexpr const Item& gen(int i) const
    {
        return floors[2 * i];
    }

    // using A* improves runtime over dijkstra by 20x
    [[nodiscard]] constexpr int aStarDistance() const
    {
        // To find min moves, consider one item permanently in the elevator, and each other item
        // only moves up. Then each item starting on floor f accounts for FLOORS - f up-moves.  The
        // net movement of the elevator is FLOORS - elevator, so factoring that out to subtract
        // later, each items up move also generates a down move for net 0
        auto elevatorNet = FLOORS - elevator();
        auto chipNGenMoves =
            FLOORS * (4 * N) - 2 * (std::ranges::fold_left(floors, 0, std::plus<>()) - elevator());
        // but we are carrying one element permanently and overcounted it, which can be any item
        // from the elevator's starting floor, i.e. another elevatorNet*2
        // there's an edgecase if only one item is on the elevator's floor and no items are below
        // it, this can count negative, so clamp
        return chipNGenMoves - std::min(3 * elevatorNet, chipNGenMoves) + steps;
    }

    constexpr auto operator<=>(const ArrayState<N>& o) const
    {
        return aStarDistance() <=> o.aStarDistance();
    }

    [[nodiscard]] constexpr bool isDone() const
    {
        return std::ranges::all_of(floors, [](auto f) { return f == FLOORS; });
    }

    [[nodiscard]] constexpr bool isValid() const
    {
        for (auto i : std::views::iota(0, N))
        {
            if (chip(i) == gen(i))
            {
                continue;
            }
            for (auto j : std::views::iota(0, N))
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
        std::vector<int> dirs;
        for (auto dir : {-1, 1})
        {
            if ((dir == -1 && elevator() <= 1) || (dir == 1 && elevator() >= FLOORS))
            {
                continue;
            }
            for (auto idx : std::views::iota(0, 2 * N))
            {
                if (floors[idx] != elevator())
                {
                    continue;
                }
                auto next = *this;
                ++next.steps;
                next.floors[idx] += dir;
                next.elevator() += dir;
                if (next.isValid())
                {
                    nexts.push_back(next);
                }
                // intuitively we should never move down with 2 items, but I can't prove that.  On
                // my input, only doing this check for dir = 1 still gives the correct answer and
                // runs more than 2x as fast, but leaving it in unless I can prove correctness
                for (auto idx2 : std::views::iota(idx + 1, 2 * N))
                {
                    if (floors[idx2] != elevator())
                    {
                        continue;
                    }
                    auto next2 = next;
                    next2.floors[idx2] += dir;
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

template <typename St> int solve(St s)
{
    std::set<typename St::Sig> seen;
    std::priority_queue<St, std::vector<St>, std::greater<>> frontier;
    seen.insert(s.sig());
    frontier.push(s);
    while (frontier.size())
    {
        St s = frontier.top();
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

template <int N> int arraySolve([[maybe_unused]] const std::vector<uint8_t>& start)
{
    if (start.size() % 2 == 0)
    {
        throw std::invalid_argument("Malformed input, missing elevator?");
    }
    if (start.size() > 2 * N + 1)
    {
        throw std::invalid_argument("Unsupported number of chips");
    }
    if (start.size() == 2 * N + 1)
    {
        ArrayState<N> state{};
        std::ranges::copy(start, state.floors.data());
        return solve(state);
    }
    return arraySolve<N - 1>(start);
}

template <> int arraySolve<0>([[maybe_unused]] const std::vector<uint8_t>& start)
{
    throw std::invalid_argument("Must be at least 1 type of chip");
}

} // namespace

template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    constexpr int MAX_CHIPS = 10;
    (void)input;
    auto part1 = arraySolve<MAX_CHIPS>({1, 1, 1, 2, 1, 2, 3, 3, 3, 3, 1});
    auto part2 = arraySolve<MAX_CHIPS>({1, 1, 1, 2, 1, 2, 3, 3, 3, 3, 1, 1, 1, 1, 1});
    return {part1, part2};
}
} // namespace aoc
