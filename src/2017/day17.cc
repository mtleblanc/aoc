#include "aoc.hh"
#include <deque>
#include <ranges>

/* https://adventofcode.com/2017/day/17
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 17;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
constexpr auto STEPS = 2017;
constexpr auto STEPS_P2 = 50'000'000;

struct Spinlock
{
    std::deque<int> values = {0};

    void insert(int steps)
    {
        while (steps-- > 0)
        {
            values.push_back(values.front());
            values.pop_front();
        }
        values.push_back(static_cast<int>(std::ssize(values)));
    }

    [[nodiscard]] int nextValue() const
    {
        return values[0];
    }
};

auto afterZero(int steps, int reps)
{
    int res{0};
    int pos{0};
    for (auto n : std::views::iota(0, reps))
    {
        pos = (pos + steps) % (n + 1);
        if (pos == 0)
        {
            res = n + 1;
        }
        ++pos;
    }
    return res;
}
} // namespace
template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    int steps{};
    input >> steps;
    Spinlock l;
    for ([[maybe_unused]] auto _ : std::views::iota(0, STEPS))
    {
        l.insert(steps);
    }
    auto part1 = l.nextValue();
    auto part2 = afterZero(steps, STEPS_P2);
    return {part1, part2};
}
} // namespace aoc
