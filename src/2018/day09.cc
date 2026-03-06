#include "aoc.hh"
#include "util.hh"
#include <algorithm>
#include <deque>

/* https://adventofcode.com/2018/day/9
 */
namespace aoc
{
constexpr size_t YEAR = 2018;
constexpr size_t DAY = 9;

template <> struct SolutionType<YEAR, DAY>
{
    using type = LongSolution;
};
using Solution = Solution_t<YEAR, DAY>;

namespace
{
template <int SCORE, int OFFSET> auto simulate(auto players, auto marbles)
{
    auto scores = std::vector<int64_t>(players);
    auto circle = std::deque<int>{0};
    for (auto next = 1, turn = 0; next < marbles; ++next, turn = (turn + 1) % players)
    {
        if (next % SCORE == 0)
        {
            scores[turn] += next;
            for ([[maybe_unused]] auto _ : std::views::iota(0, OFFSET))
            {
                circle.push_front(circle.back());
                circle.pop_back();
            }
            scores[turn] += circle.back();
            circle.pop_back();
            circle.push_back(circle.front());
            circle.pop_front();
        }
        else
        {
            circle.push_back(circle.front());
            circle.pop_front();
            circle.push_back(next);
        }
    }
    return std::ranges::max(scores);
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto numbers = readNumbers(slurp(input));
    if (numbers.size() != 2)
    {
        throw std::invalid_argument{"Malformed input"};
    }
    auto players = numbers[0];
    auto marbles = numbers[1];

    static constexpr auto SCORE = 23;
    static constexpr auto OFFSET = 7;
    static constexpr auto P2_MUL = 100;
    return {simulate<SCORE, OFFSET>(players, marbles),
            simulate<SCORE, OFFSET>(players, marbles * P2_MUL)};
}
} // namespace aoc
