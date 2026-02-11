#include "aoc.hh"
#include <algorithm>
#include <cassert>
#include <ctre.hpp>
#include <queue>
#include <ranges>
#include <set>
#include <utility>

/* https://adventofcode.com/2016/day/22
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 22;

namespace
{
struct Node
{
    int x;
    int y;
    int cap;
    int used;

    auto operator<=>(const Node& o) const = default;
};

[[maybe_unused]] std::istream& operator>>(std::istream& is, Node& n)
{
    constexpr auto DF =
        ctll::fixed_string(R"(/dev/grid/node-x(\d+)-y(\d+)\s*(\d+)T\s*(\d+)T\s*(\d+)T\s*(\d+)%)");
    constexpr auto X = 1;
    constexpr auto Y = 2;
    constexpr auto CAP = 3;
    [[maybe_unused]] constexpr auto USED = 4;
    [[maybe_unused]] constexpr auto FREE = 5;
    [[maybe_unused]] constexpr auto PCT = 6;
    std::string line;
    std::getline(is, line);
    if (auto m = ctre::match<DF>(line))
    {
        n.x = m.get<X>().to_number();
        n.y = m.get<Y>().to_number();
        n.cap = m.get<CAP>().to_number();
        n.used = m.get<USED>().to_number();
        return is;
    }
    is.setstate(std::ios_base::failbit);
    return is;
}

auto canMove(const Node& from)
{
    return [&from](const Node& to)
    { return from != to && from.used > 0 && to.cap - to.used >= from.used; };
}

auto countValid(const std::vector<Node>& nodes)
{
    return std::ranges::fold_left(
        nodes | std::views::transform([&nodes](auto n)
                                      { return std::ranges::count_if(nodes, canMove(n)); }),
        0, std::plus<>());
}

// It doesn't seem viable to solve this problem in general, but the input is very constrained
//  * One node is empty and is the only node that has space for any other node's data
//  * A handful of nodes are large and nearly full and no node has free space to take their data
//  * The remaining nodes fit into empty, and after moving their data, they become the empty node
//  with the above two properties still holding
//
// So this is just a sliding block puzzle with some blocks fixed.  I imagine the shortest solution
// for a full ~1000 block puzzle is also infeasible, but to just solve one piece we can just do a
// bfs or A*

struct PuzzleState
{
    std::pair<int, int> empty;
    std::pair<int, int> target;
    auto operator<=>(const PuzzleState& o) const = default;
};

class PuzzleSearch
{
    int estimate{};
    int steps_;
    PuzzleState s_;
    void updateEstimate();

  public:
    PuzzleSearch(PuzzleState s, int steps = 0) : steps_{steps}, s_{std::move(s)}
    {
        updateEstimate();
    }
    auto operator<=>(const PuzzleSearch& o) const = default;
    [[nodiscard]] auto isDone() const
    {
        return s_.target == std::pair(0, 0);
    }
    [[nodiscard]] auto steps() const
    {
        return steps_;
    }
    [[nodiscard]] auto state() const
    {
        return s_;
    }
};

void PuzzleSearch::updateEstimate()
{
    // we need to at least move the empty square to target
    // which will move target once
    // to move target again we need at least 3 more moves (if moving perpendicular to last move)
    if (isDone())
    {
        estimate = steps_;
        return;
    }
    auto distToTar =
        std::abs(s_.target.first - s_.empty.first) + std::abs(s_.target.second - s_.empty.second);
    auto tarToOrigin = s_.target.first + s_.target.second;
    estimate = steps_ + distToTar + 3 * (tarToOrigin - 1);
}

std::pair<int, int> operator+(const std::pair<int, int>& l, const std::pair<int, int>& r)
{
    return {l.first + r.first, l.second + r.second};
}

auto solveSlidingBlock(const std::vector<Node>& nodes)
{
    auto empty = std::ranges::find_if(nodes, [](auto n) { return n.used == 0; });
    auto target = std::ranges::max(nodes | std::views::filter([](auto n) { return n.y == 0; }) |
                                   std::views::transform(&Node::x));
    auto space = empty->cap;
    auto valid = nodes | std::views::filter([space](auto n) { return n.used <= space; }) |
                 std::views::transform([](auto n) { return std::make_pair(n.x, n.y); }) |
                 std::ranges::to<std::set>();
    std::set<PuzzleState> seen;
    std::priority_queue<PuzzleSearch, std::vector<PuzzleSearch>, std::greater<>> horizon;
    PuzzleState start{.empty = {empty->x, empty->y}, .target = {target, 0}};
    seen.insert(start);
    horizon.emplace(start);
    while (horizon.size())
    {
        auto cur = horizon.top();
        horizon.pop();
        if (cur.isDone())
        {
            return cur.steps();
        }
        constexpr auto DIRS =
            std::array<std::pair<int, int>, 4>{{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};
        for (auto d : DIRS)
        {
            auto state = cur.state();
            auto newEmpty = state.empty + d;
            if (!valid.contains(newEmpty))
            {
                continue;
            }
            auto newTarget = newEmpty == state.target ? state.empty : state.target;
            PuzzleState newState{.empty = newEmpty, .target = newTarget};
            if (seen.contains(newState))
            {
                continue;
            }
            seen.insert(newState);
            horizon.emplace(newState, cur.steps() + 1);
        }
    }
    return -1;
}

} // namespace

template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    std::string tmp;
    // 2 header lines
    std::getline(input, tmp);
    std::getline(input, tmp);
    auto nodes = readAll<Node>(input);
    assert(input.eof());
    return {static_cast<int>(countValid(nodes)), solveSlidingBlock(nodes)};
}
} // namespace aoc
