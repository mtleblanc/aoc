#include "aoc.hh"
#include <algorithm>

/* https://adventofcode.com/2016/day/2
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 2;

namespace
{
ssize_t gridAt(ssize_t x, ssize_t y)
{
    // NOLINTNEXTLINE (cppcoreguidelines-avoid-magic-numbers)
    return 7 + x - 3 * y;
}
ssize_t part1(std::vector<std::string> const& movements)
{
    int x{1};
    int y{1};
    ssize_t code{};
    for (auto const& m : movements)
    {
        for (auto c : m)
        {
            switch (c)
            {
            case 'U':
                ++y;
                break;
            case 'D':
                --y;
                break;
            case 'L':
                --x;
                break;
            case 'R':
                ++x;
                break;
            default:
                throw std::invalid_argument("Unknown direction");
            }
            x = std::clamp(x, 0, 2);
            y = std::clamp(y, 0, 2);
        }
        // NOLINTNEXTLINE (cppcoreguidelines-avoid-magic-numbers)
        code *= 10;
        code += gridAt(x, y);
    }
    return code;
}

/* Grid is now
    1
  2 3 4
5 6 7 8 9
  A B C
    D
*/
ssize_t part2(std::vector<std::string> const& movements)
{
    constexpr ssize_t N = 7;
    constexpr ssize_t HOME_X = 1;
    constexpr ssize_t HOME_Y = 3;
    // 0 represents OOB
    // clang-format off
    constexpr std::array<ssize_t, N*N> GRID {
        0, 0, 0, 0, 0, 0, 0,
        0, 0, 0,13, 0, 0, 0,
        0, 0,10,11,12, 0, 0,
        0, 5, 6, 7, 8, 9, 0,
        0, 0, 2, 3, 4, 0, 0,
        0, 0, 0, 1, 0, 0, 0
    };
    // clang-format on
    auto at = [GRID](ssize_t x, ssize_t y) { return GRID[x + N * y]; };
    ssize_t x{HOME_X};
    ssize_t y{HOME_Y};
    ssize_t code{};
    for (auto const& m : movements)
    {
        for (auto c : m)
        {
            switch (c)
            {
            case 'U':
                if (at(x, y + 1))
                {
                    ++y;
                }
                break;
            case 'D':
                if (at(x, y - 1))
                {
                    --y;
                }
                break;
            case 'L':
                if (at(x - 1, y))
                {
                    --x;
                }
                break;
            case 'R':
                if (at(x + 1, y))
                {
                    ++x;
                }
                break;
            default:
                throw std::invalid_argument("Unknown direction");
            }
        }
        // NOLINTNEXTLINE (cppcoreguidelines-avoid-magic-numbers)
        code *= 16;
        code += at(x, y);
    }
    return code;
}
} // namespace

template <> struct SolutionType<YEAR, DAY>
{
    using type = aoc::StringSolution;
};

template <> StringSolution solve<YEAR, DAY>(std::istream& input)
{
    auto movements = readAllLines(input);
    return {std::to_string(part1(movements)), toHex(part2(movements))};
}
} // namespace aoc
