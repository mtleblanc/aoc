#include "aoc.hh"
#include "util.hh"
#include <algorithm>
#include <ranges>

/* https://adventofcode.com/2018/day/14
 */
namespace aoc
{
constexpr size_t YEAR = 2018;
constexpr size_t DAY = 14;

template <> struct SolutionType<YEAR, DAY>
{
    using type = LongSolution;
};
using Solution = Solution_t<YEAR, DAY>;

namespace
{
constexpr auto B = 10;

struct Scoreboard
{
    // NOLINTNEXTLINE(*-magic-numbers)
    std::vector<int> scoreboard{3, 7};
    int64_t elf1 = 0Z;
    int64_t elf2 = 1Z;

    auto makeRecipes()
    {
        auto nextScore = scoreboard[elf1] + scoreboard[elf2];
        if (nextScore >= B)
        {
            scoreboard.push_back(nextScore / B);
            nextScore -= B;
        }
        scoreboard.push_back(nextScore);
        elf1 = (elf1 + 1 + scoreboard[elf1]) % std::ssize(scoreboard);
        elf2 = (elf2 + 1 + scoreboard[elf2]) % std::ssize(scoreboard);
    }
};

// This could be done as part of part2, but practice is on the order of 70k while part2 returns an
// answer on the order of 20m, so the duplicate work is immaterial
auto part1(auto practice)
{
    static constexpr auto N = 10;
    auto scoreboard = Scoreboard{};
    while (std::ssize(scoreboard.scoreboard) < practice + N)
    {
        scoreboard.makeRecipes();
    }
    auto resScores = scoreboard.scoreboard | std::views::drop(practice) | std::views::take(N);
    return std::ranges::fold_left(resScores, int64_t{0},
                                  [](auto acc, auto n) { return B * acc + n; });
}

auto part2(const auto& target)
{
    auto scoreboard = Scoreboard{};
    auto index = int64_t{0};
    for (;;)
    {
        scoreboard.makeRecipes();
        while (index + std::ssize(target) < std::ssize(scoreboard.scoreboard))
        {
            if (std::ranges::equal(target, scoreboard.scoreboard | std::views::drop(index) |
                                               std::views::take(std::ssize(target))))
            {
                return index;
            }
            ++index;
        }
    }
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto target = std::string{};
    input >> target;
    auto practice = toNum(target);
    auto targetVec = std::ranges::to<std::vector>(
        target | std::views::transform([](auto c) { return c - '0'; }));
    return {part1(practice), part2(targetVec)};
}
} // namespace aoc
