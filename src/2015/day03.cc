#include "aoc.hh"
#include <algorithm> // IWYU pragma: keep - apple clangd seems to think fold_left is in <ranges>
#include <ranges>
#include <set>

/* https://adventofcode.com/2015/day/3
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 3;

namespace
{
template <size_t N> struct Locations
{
    using Loc = std::pair<int, int>;
    std::array<Loc, N> current;
    size_t idx{};
    std::set<Loc> visited = {current[0]};
};

template <size_t N> Locations<N> step(Locations<N> ls, typename Locations<N>::Loc delta)
{
    ls.current[ls.idx].first += delta.first;
    ls.current[ls.idx].second += delta.second;
    ls.visited.insert(ls.current[ls.idx]);
    ls.idx = (ls.idx + 1) % N;
    return ls;
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto toMoves = [](auto c)
    {
        switch (c)
        {
        case '^':
            return std::make_pair(0, 1);
        case 'v':
            return std::make_pair(0, -1);
        case '>':
            return std::make_pair(1, 0);
        case '<':
            return std::make_pair(-1, 0);
        default:
            throw std::invalid_argument("Unknown character");
        }
    };

    auto instructions = slurp(input);
    auto view = instructions | std::views::transform(toMoves);
    auto part1Result = std::ranges::fold_left(view, Locations<1>{}, step<1>);
    auto part2Result = std::ranges::fold_left(view, Locations<2>{}, step<2>);

    return {std::ssize(part1Result.visited), std::ssize(part2Result.visited)};
}
} // namespace aoc
