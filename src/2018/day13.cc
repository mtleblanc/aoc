#include "aoc.hh"
#include "coords.hh"
#include <algorithm>
#include <ranges>

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
struct Cart
{
    std::pair<int, int> loc;
    std::pair<int, int> dir;
    int turn = -1;

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

auto firstCrash(const auto& maze, auto carts)
{
    std::ranges::sort(carts);
    for (;;)
    {
        for (auto& c : carts)
        {
            auto nextLoc = c.loc + c.dir;
            if (std::ranges::any_of(carts, [=](auto c) { return c.loc == nextLoc; }))
            {
                return std::format("{},{}", nextLoc.second, nextLoc.first);
            }
            c.loc = nextLoc;
            switch (maze[c.loc])
            {
            case '|':
            case '-':
                break;
            case '/':
            {
                auto [y, x] = c.dir;
                c.dir = std::make_pair(x, y);
                break;
            }
            case '\\':
            {
                auto [y, x] = c.dir;
                c.dir = std::make_pair(-x, -y);
                break;
            }
            case '+':
            {
                if (c.turn == 0)
                {
                    ++c.turn;
                    break;
                }
                auto [y, x] = c.dir;
                c.dir = std::make_pair(-c.turn * x, c.turn * y);
                ++c.turn;
                if (c.turn == 2)
                {
                    c.turn -= 2;
                }
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
            carts.emplace_back(maze.index(i), std::make_pair(-1, 0));
            break;
        case '<':
            maze[i] = '-';
            carts.emplace_back(maze.index(i), std::make_pair(0, -1));
            break;
        case 'v':
            maze[i] = '|';
            carts.emplace_back(maze.index(i), std::make_pair(1, 0));
            break;
        case '>':
            maze[i] = '-';
            carts.emplace_back(maze.index(i), std::make_pair(0, 1));
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
    return {firstCrash(maze, carts)};
}
} // namespace aoc
