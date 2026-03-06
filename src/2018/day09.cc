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
void rotateRight(auto& deque)
{
    deque.push_front(deque.back());
    deque.pop_back();
}

void rotateLeft(auto& deque)
{
    deque.push_back(deque.front());
    deque.pop_front();
}

constexpr auto SCORE = 23;
constexpr auto OFFSET = 7;
auto simulate(auto players, auto marbles)
{
    auto scores = std::vector<int64_t>(players);
    auto circle = std::deque<int>{0};
    for (auto next = 1, turn = 0; next < marbles; ++next, turn = (turn + 1) % players)
    {
        if (next % SCORE == 0)
        {
            scores[turn] += next;
            for (auto i = 0; i < OFFSET; ++i)
            {
                rotateRight(circle);
            }
            scores[turn] += circle.back();
            circle.pop_back();
            rotateLeft(circle);
        }
        else
        {
            rotateLeft(circle);
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

    static constexpr auto P2_MUL = 100;
    return {simulate(players, marbles), simulate(players, marbles * P2_MUL)};
}
} // namespace aoc
