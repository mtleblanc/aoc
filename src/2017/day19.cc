#include "aoc.hh"
#include <cassert>

/* https://adventofcode.com/2017/day/19
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 19;

template <> struct SolutionType<YEAR, DAY>
{
    using type = StringSolution;
};
using Solution = Solution_t<YEAR, DAY>;

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto maze = slurp(input);
    auto stride = std::distance(maze.begin(), std::ranges::find(maze, '\n')) + 1;
    auto cur = std::distance(maze.begin(), std::ranges::find(maze, '|'));
    auto dir = stride;
    auto res = std::string{};
    auto steps{1};
    for (bool done{}; !done;)
    {
        cur += dir;
        ++steps;
        auto c = maze[cur];
        if (std::isalpha(c))
        {
            res.push_back(c);
        }
        // very lazy, don't check for turns or boundaries
        if (maze[cur + dir] != ' ')
        {
            continue;
        }
        done = true;
        auto back = -dir;
        for (auto d : {-stride, stride, 1Z, -1Z})
        {
            if (d != back && maze[cur + d] != ' ')
            {
                assert(done);
                done = false;
                dir = d;
            }
        }
    }
    return {res, std::to_string(steps)};
}
} // namespace aoc
