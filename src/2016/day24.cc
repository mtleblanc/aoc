#include "aoc.hh"
#include <algorithm>
#include <cassert>
#include <deque>
#include <memory>
#include <ranges>
#include <set>

/* https://adventofcode.com/2016/day/24
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 24;

namespace
{

template <typename Loc, typename Visited> struct State
{
    Loc loc;
    Visited visited;
    auto operator<=>(const State<Loc, Visited>& o) const = default;
};

template <typename Loc, typename Visited> struct Path
{
    int steps;
    State<Loc, Visited> state;
};

template <typename T, std::unsigned_integral I = unsigned> class VisitSet
{
    std::shared_ptr<std::vector<T>> targets_;
    I seen_{};

  public:
    VisitSet(const std::shared_ptr<std::vector<T>>& targets) : targets_{targets}
    {
        assert(targets->size() + 1 <= std::numeric_limits<I>::digits);
        seen_ = (1U << targets->size()) - 1U;
    }

    [[nodiscard]] bool isDone() const
    {
        return seen_ == 0;
    }

    [[nodiscard]] VisitSet<T, I> visit(T t) const
    {
        auto bit = 1U << std::ranges::distance(targets_->begin(), std::ranges::find(*targets_, t));
        if (!(seen_ & bit))
        {
            return *this;
        }
        auto ret = *this;
        ret.seen_ &= ~bit;
        return ret;
    }

    auto operator<=>(const VisitSet<T, I>& o) const
    {
        return seen_ <=> o.seen_;
    }
    auto operator==(const VisitSet<T, I>& o) const
    {
        return ((*this) <=> o) == std::strong_ordering::equal;
    }
};
} // namespace

template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    const auto maze = slurp(input);
    const auto stride = std::ranges::distance(maze.begin(), std::ranges::find(maze, '\n')) + 1;
    assert(maze.size() % stride == 0);
    const auto start = std::ranges::distance(maze.begin(), std::ranges::find(maze, '0'));
    auto targets = std::ranges::to<std::vector>(
        maze | std::views::filter([](auto c) { return c != '.' && c != '#' && c != '\n'; }));
    std::ranges::sort(targets);
    const auto ptargets = std::make_shared<decltype(targets)>(std::move(targets));

    auto seen = std::set<State<ssize_t, VisitSet<char>>>{};
    auto horizon = std::deque<Path<ssize_t, VisitSet<char>>>{};

    const auto initial = State{.loc = start, .visited = VisitSet{ptargets}.visit('0')};
    seen.insert(initial);
    horizon.emplace_front(0, initial);
    const auto neighbours = {-1Z, 1Z, stride, -stride};
    std::optional<int> part1;

    while (horizon.size())
    {
        const auto& cur = horizon.front();
        if (!part1 && cur.state.visited.isDone())
        {
            part1 = cur.steps;
        }
        if (cur.state.visited.isDone() && maze[cur.state.loc] == '0')
        {
            return {*part1, cur.steps};
        }
        for (auto d : neighbours)
        {
            auto loc = d + cur.state.loc;
            if (maze[loc] == '#')
            {
                continue;
            }
            auto state = State{.loc = loc,
                               .visited = maze[loc] == '.' ? cur.state.visited
                                                           : cur.state.visited.visit(maze[loc])};
            if (seen.contains(state))
            {
                continue;
            }
            seen.insert(state);
            horizon.emplace_back(cur.steps + 1, state);
        }
        horizon.pop_front();
    }

    return {};
}
} // namespace aoc
