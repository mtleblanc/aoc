#include "aoc.hh"
#include <algorithm>
#include <iostream>
#include <print>
#include <ranges>
#include <stdexcept>

/* https://adventofcode.com/2018/day/12
 */
namespace aoc
{
constexpr size_t YEAR = 2018;
constexpr size_t DAY = 12;

template <> struct SolutionType<YEAR, DAY>
{
    using type = LongSolution;
};
using Solution = Solution_t<YEAR, DAY>;

namespace
{
template <int64_t REPS> auto simulate(const auto& rules, const auto& initialState)
{
    static constexpr auto MASK = (1 << 5) - 1;
    if (rules[0] == 1)
    {
        // not simulating the case where there are infinite plants after step 1, though we could do
        // so by tracking the infinite state and starting rule as 0b11111 when the infinite state is
        // 1.  For the return value to be finite, we'd also need rule[31] = 0 so that there are
        // finite plants on even steps
        throw std::invalid_argument{"Plants appear from nothing"};
    }

    // pot number corresponding to state[0]
    auto offset = int64_t{0};
    auto state = std::vector<int>(initialState);
    auto nextState = std::vector<int>();
    for (auto r = 0; r < REPS; ++r)
    {
        nextState.clear();
        auto shift = int64_t{-2};
        // easiest way to deal with the end is just to make sure we get to all 0, but we need to pop
        // these back off later the short circuit check
        auto stateSize = state.size();
        state.resize(stateSize + 4);
        auto rule = 0;
        auto found = false;
        for (auto s : state)
        {
            rule = (rule * 2 + s) & MASK;
            if (found)
            {
                nextState.push_back(rules[rule]);
            }
            else
            {
                auto cur = rules[rule];
                if (cur == 0)
                {
                    ++shift;
                }
                else
                {
                    found = true;
                    nextState.push_back(cur);
                }
            }
        }
        state.resize(stateSize);
        while (!nextState.empty() && nextState.back() == 0)
        {
            nextState.pop_back();
        }
        if (state == nextState)
        {
            // NB state hasn't been replaced with newState yet
            offset += shift * (REPS - r);
            break;
        }
        std::swap(state, nextState);
        offset += shift;
    }
    auto indexIfAlive = [](auto p)
    {
        auto [n, s] = p;
        return s ? n : 0;
    };
    using namespace std::views;
    return std::ranges::fold_left(zip(iota(offset), state) | transform(indexIfAlive), int64_t{0},
                                  std::plus{});
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    static constexpr auto WINDOW = 5;
    static constexpr auto RULES = 1 << WINDOW;
    static constexpr auto INITIAL_PREFIX = std::string_view{"initial state: "}.size();
    static constexpr int64_t P1_REPS = 20;
    static constexpr int64_t P2_REPS = 50'000'000'000;

    auto value = [](auto c) { return c == '#' ? 1 : 0; };

    auto lines = readAllLines(input);
    if (lines.size() != RULES + 2 || !lines[1].empty())
    {
        throw std::invalid_argument{"Malformed input"};
    }

    auto state = std::ranges::to<std::vector>(lines[0].substr(INITIAL_PREFIX) |
                                              std::views::transform(value));
    std::array<int, RULES> rules{};
    for (const auto& r : lines | std::views::drop(2))
    {
        auto index = 0;
        for (auto c : r | std::views::take(WINDOW))
        {
            index = index * 2 + value(c);
        }
        rules[index] = value(r.back());
    }

    return {simulate<P1_REPS>(rules, state), simulate<P2_REPS>(rules, state)};
}
} // namespace aoc
