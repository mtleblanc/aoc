#include "aoc.hh"
#include "util.hh"
#include <algorithm>
#include <iterator>
#include <print>
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

class Scoreboard : std::ranges::view_interface<Scoreboard>
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

  public:
    class Iterator
    {
        Scoreboard* sb;
        int64_t index = 0;
        Iterator(Scoreboard* sb) : sb{sb} {}
        friend class Scoreboard;

      public:
        using difference_type = int64_t;
        using value_type = int;
        using reference = int;
        using iterator_category = std::random_access_iterator_tag;
        auto operator*() const
        {
            while (std::ssize(sb->scoreboard) <= index)
            {
                sb->makeRecipes();
            }
            return sb->scoreboard[index];
        }

        auto& operator++()
        {
            ++index;
            return (*this);
        }

        auto operator++(int)
        {
            auto res = *this;
            return ++res;
        }

        auto& operator--()
        {
            --index;
            return (*this);
        }

        auto operator--(int)
        {
            auto res = *this;
            return --res;
        }

        auto operator-(Iterator o) const
        {
            return index - o.index;
        }

        auto operator<=>(const Iterator& o) const
        {
            return index <=> o.index;
        }
    };

    Scoreboard() = default;
    Scoreboard(const Scoreboard&) = delete;
    Scoreboard(Scoreboard&&) = default;
    Scoreboard& operator=(const Scoreboard&) = delete;
    Scoreboard& operator=(Scoreboard&&) = default;
    ~Scoreboard() = default;

    auto begin()
    {
        return Iterator{this};
    }

    static auto end()
    {
        return std::unreachable_sentinel;
    }
};

auto part1(auto practice, auto& scoreboard)
{
    static constexpr auto N = 10;
    auto resScores = scoreboard | std::views::drop(practice) | std::views::take(N);
    return std::ranges::fold_left(resScores, int64_t{0},
                                  [](auto acc, auto n) { return B * acc + n; });
}

auto part2(const auto& target, auto& scoreboard)
{
    auto it = scoreboard.begin();

    for (;; ++it)
    {
        if (std::ranges::equal(target, std::views::counted(it, std::ssize(target))))
        {
            return it - scoreboard.begin();
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
    auto scoreboard = Scoreboard{};
    return {part1(practice, scoreboard), part2(targetVec, scoreboard)};
}
} // namespace aoc
