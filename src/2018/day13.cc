#include "aoc.hh"
#include "coords.hh"
#include <algorithm>
#include <ranges>
#include <utility>

/* https://adventofcode.com/2018/day/13
 */
namespace aoc
{
constexpr size_t YEAR = 2018;
constexpr size_t DAY = 13;

template <> struct SolutionType<YEAR, DAY>
{
    using type = StringSolution;
};
using Solution = Solution_t<YEAR, DAY>;

namespace
{
struct Direction
{

    enum class Turn
    {
        LEFT,
        STRAIGHT,
        RIGHT,
        REVERSE,
        COUNT
    };

    // y,x indexing so that sort is reading order
    static constexpr auto UP = std::make_pair(-1, 0);
    static constexpr auto LEFT = std::make_pair(0, -1);
    static constexpr auto RIGHT = std::make_pair(0, 1);
    static constexpr auto DOWN = std::make_pair(1, 0);

    static constexpr auto doTurn(std::pair<int, int> direction, Turn turn) -> std::pair<int, int>
    {
        auto [y, x] = direction;
        switch (turn)
        {
        case Turn::LEFT:
            return {-x, y};
        case Turn::STRAIGHT:
            return direction;
        case Turn::RIGHT:
            return {x, -y};
        case Turn::REVERSE:
            return {-y, -x};
        case Turn::COUNT:
            throw std::logic_error{"Invalid turn value"};
        default:
            std::unreachable();
        }
    }

    static constexpr auto doCorner(std::pair<int, int> direction, char c) -> std::pair<int, int>
    {
        if (c != '\\' && c != '/')
        {
            throw std::invalid_argument{"Invalid corner"};
        }
        auto [y, x] = direction;
        if (c == '\\')
        {
            return {x, y};
        }
        return {-x, -y};
    }
};

struct Cart
{
    std::pair<int, int> loc;
    std::pair<int, int> dir;
    int turnIndex = 0;
    bool alive = true;
    constexpr static std::array<Direction::Turn, 3> TURNS = {
        {Direction::Turn::LEFT, Direction::Turn::STRAIGHT, Direction::Turn::RIGHT}};

    [[nodiscard]] Direction::Turn turn() const
    {
        return TURNS[turnIndex % std::ssize(TURNS)];
    }

    auto operator<=>(const Cart&) const = default;
};

class Maze
{
    std::string maze_;
    int width_;

  public:
    Maze(std::string m) : maze_{std::move(m)}, width_{static_cast<int>(maze_.find('\n') + 1)} {}
    // y,x indexing so that sort is reading order
    auto index(auto i)
    {
        return std::make_pair(i / width_, i % width_);
    };

    auto& operator[](int i)
    {
        return maze_[i];
    }
    auto& operator[](int y, int x)
    {
        return maze_[y * width_ + x];
    }
    auto& operator[](std::pair<int, int> coord)
    {
        auto [x, y] = coord;
        return operator[](x, y);
    }
    auto& operator[](int i) const
    {
        return maze_[i];
    }
    auto& operator[](int y, int x) const
    {
        return maze_[y * width_ + x];
    }
    auto& operator[](std::pair<int, int> coord) const
    {
        auto [x, y] = coord;
        return operator[](x, y);
    }

    auto size()
    {
        return std::ssize(maze_);
    }
};

auto simulate(const auto& maze, auto carts)
{
    auto found = false;
    auto res = StringSolution{};
    for (;;)
    {
        std::ranges::sort(carts);
        carts = std::ranges::to<std::vector>(carts |
                                             std::views::filter([](auto& c) { return c.alive; }));
        if (carts.size() == 1)
        {
            auto [y, x] = carts.front().loc;
            res.part2 = std::format("{},{}", x, y);
            return res;
        }
        for (auto& c : carts)
        {
            if (!c.alive)
            {
                continue;
            }
            auto nextLoc = c.loc + c.dir;
            // because we check on each update, at most one crash
            auto crash =
                std::ranges::find_if(carts, [=](auto c) { return c.alive && c.loc == nextLoc; });
            if (crash != carts.end())
            {
                if (!found)
                {
                    found = true;
                    res.part1 = std::format("{},{}", nextLoc.second, nextLoc.first);
                }
                c.alive = false;
                crash->alive = false;
                continue;
            }
            c.loc = nextLoc;
            switch (maze[c.loc])
            {
            case '|':
            case '-':
                break;
            case '\\':
            case '/':
            {
                c.dir = Direction::doCorner(c.dir, maze[c.loc]);
                break;
            }
            case '+':
            {
                c.dir = Direction::doTurn(c.dir, c.turn());
                ++c.turnIndex;
                break;
            }
            default:
                throw std::logic_error{"off route"};
            }
        }
    }
}
} // namespace
template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto maze = Maze{slurp(input)};
    auto carts = std::vector<Cart>{};
    for (auto i = 0; i < maze.size(); ++i)
    {
        switch (maze[i])
        {
        case '^':
            maze[i] = '|';
            carts.emplace_back(maze.index(i), Direction::UP);
            break;
        case '<':
            maze[i] = '-';
            carts.emplace_back(maze.index(i), Direction::LEFT);
            break;
        case 'v':
            maze[i] = '|';
            carts.emplace_back(maze.index(i), Direction::DOWN);
            break;
        case '>':
            maze[i] = '-';
            carts.emplace_back(maze.index(i), Direction::RIGHT);
            break;
        case '+':
        case '-':
        case '|':
        case '\\':
        case '/':
        case ' ':
        case '\n':
            break;
        default:
            throw std::invalid_argument{"Unrecognized character"};
        }
    }
    return simulate(maze, carts);
}
} // namespace aoc
