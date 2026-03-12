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
enum class Turn
{
    LEFT,
    STRAIGHT,
    RIGHT,
};

// y,x indexing so that sort is reading order
constexpr auto UP = std::make_pair(-1, 0);
constexpr auto LEFT = std::make_pair(0, -1);
constexpr auto RIGHT = std::make_pair(0, 1);
constexpr auto DOWN = std::make_pair(1, 0);

constexpr auto applyTurn(std::pair<int, int> direction, Turn turn) -> std::pair<int, int>
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
    default:
        std::unreachable();
    }
}

struct Cart
{
    constexpr static std::array<Turn, 3> TURNS = {{Turn::LEFT, Turn::STRAIGHT, Turn::RIGHT}};

    std::pair<int, int> loc;
    std::pair<int, int> dir;
    int turnIndex = 0;
    bool alive = true;

    Cart(std::pair<int, int> loc, std::pair<int, int> dir) : loc{loc}, dir{dir} {}
    void turn(char m)
    {
        auto [y, x] = dir;
        switch (m)
        {
        case '|':
        case '-':
            break;
        case '\\':
            dir = {x, y};
            break;
        case '/':
        {
            dir = {-x, -y};
            break;
        }
        case '+':
        {
            dir = applyTurn(dir, turnDecision());
            ++turnIndex;
            break;
        }
        default:
            throw std::logic_error{"off route"};
        }
    }
    [[nodiscard]] Turn turnDecision() const
    {
        return TURNS[turnIndex % std::ssize(TURNS)];
    }

    auto operator<=>(const Cart& o) const
    {
        return loc <=> o.loc;
    }

    bool operator==(const Cart& o) const
    {
        return (*this <=> o) == std::strong_ordering::equal;
    }
};

class Maze
{
    std::string maze_;
    int width_;

  public:
    Maze(std::string m) : maze_{std::move(m)}, width_{static_cast<int>(maze_.find('\n') + 1)} {}
    // y,x indexing so that sort is reading order
    [[nodiscard]] auto index(auto i) const
    {
        return std::make_pair(i / width_, i % width_);
    }

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
        auto [y, x] = coord;
        return operator[](y, x);
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
        auto [y, x] = coord;
        return operator[](y, x);
    }

    [[nodiscard]] auto size() const
    {
        return std::ssize(maze_);
    }
};

auto simulate(const auto& maze, auto carts)
{
    auto foundFirstCollision = false;
    auto res = StringSolution{};
    for (;;)
    {
        carts = std::ranges::to<std::vector>(carts |
                                             std::views::filter([](auto& c) { return c.alive; }));
        std::ranges::sort(carts);
        if (carts.size() == 0)
        {
            throw std::invalid_argument{"No carts remain"};
        }
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
                if (!foundFirstCollision)
                {
                    foundFirstCollision = true;
                    res.part1 = std::format("{},{}", nextLoc.second, nextLoc.first);
                }
                c.alive = false;
                crash->alive = false;
                continue;
            }
            c.loc = nextLoc;
            c.turn(maze[c.loc]);
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
            carts.emplace_back(maze.index(i), UP);
            break;
        case '<':
            maze[i] = '-';
            carts.emplace_back(maze.index(i), LEFT);
            break;
        case 'v':
            maze[i] = '|';
            carts.emplace_back(maze.index(i), DOWN);
            break;
        case '>':
            maze[i] = '-';
            carts.emplace_back(maze.index(i), RIGHT);
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
