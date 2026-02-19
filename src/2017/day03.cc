#include "aoc.hh"
#include <cassert>
#include <utility>

/* https://adventofcode.com/2017/day/3
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 3;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
constexpr auto location(int n)
{
    auto loc = std::make_pair(0, 0);
    int cur = 1;
    int step = 1;
    while (cur + 4 * step + 2 <= n)
    {
        --loc.first;
        --loc.second;
        cur += 4 * step + 2;
        step += 2;
    }
    auto rem = n - cur;
    auto right = std::min(rem, step);
    loc.first += right;
    rem -= right;
    auto up = std::min(rem, step);
    loc.second += up;
    rem -= up;
    auto left = std::min(rem, step + 1);
    loc.first -= left;
    rem -= left;
    auto down = std::min(rem, step + 1);
    loc.second -= down;
    return loc;
}

auto pascal(int target)
{
    // every corner at least doubles, +2 so we can sum around without bounds checking, only
    // necessary for small input but kept in since it runs in microseconds anyway
    auto gridSize =
        std::numeric_limits<unsigned>::digits - std::countl_zero(static_cast<unsigned>(target)) + 2;
    std::vector<int> spiral(gridSize * gridSize);
    auto stride = static_cast<int>(gridSize);
    auto loc = gridSize / 2 * (gridSize + 1);
    spiral[loc] = 1;
    auto dir = 1;
    auto remaining = 1;
    auto doubleStep = 2;
    std::vector dirs{-1, -1 + stride, stride, 1 + stride, 1, 1 - stride, -stride, -1 - stride};
    for (;;)
    {
        loc += dir;
        if (--remaining == 0)
        {
            ++doubleStep;
            remaining = doubleStep / 2;
            switch (doubleStep % 4)
            {
            case 0:
                dir = -1;
                break;
            case 1:
                dir = stride;
                break;
            case 2:
                dir = 1;
                break;
            case 3:
                dir = -stride;
                break;
            default:
                std::unreachable();
            }
        }
        for (auto d : dirs)
        {
            assert(loc + d < std::ssize(spiral));
            spiral[loc] += spiral[loc + d];
        }
        if (spiral[loc] > target)
        {
            return spiral[loc];
        }
    }
}

} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    int target{};
    input >> target;
    auto [x, y] = location(target);
    return {std::abs(x) + std::abs(y), pascal(target)};
}
} // namespace aoc
