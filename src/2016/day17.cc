#include "aoc.hh"
#include "hash.hh"
#include <deque>
#include <optional>
#include <ranges>

/* https://adventofcode.com/2016/day/17
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 17;

namespace
{
struct Path
{
    int x{};
    int y{};
    std::string path;
};

template <int GRID> StringSolution bfs(const std::string& seed)
{
    constexpr int MAX = GRID - 1;
    constexpr std::array<std::pair<int, int>, 4> DIRS = {
        {/*UP*/ {0, -1}, /*DOWN*/ {0, 1}, /*LEFT*/ {-1, 0}, /*RIGHT*/ {1, 0}}};
    constexpr std::string_view DIR_LITERALS = "UDLR";
    auto hasher = Hash::Hasher::md5Hasher();
    std::deque<Path> horizon;
    horizon.emplace_back();
    std::optional<std::string> shortest;
    size_t longest{};
    while (horizon.size())
    {
        auto cur = horizon.front();
        horizon.pop_front();
        auto hash = hasher(seed + cur.path);
        // NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers)
        std::array<int, 4> locks = {hash[0] >> 4, hash[0] & 0x0F, hash[1] >> 4, hash[1] & 0x0F};
        for (auto [dir, d, l] : std::views::zip(DIRS, DIR_LITERALS, locks) | std::views::reverse)
        {
            if (l >= 0x0B)
            {
                auto [dx, dy] = dir;
                auto path = cur.path + d;
                auto x = cur.x + dx;
                auto y = cur.y + dy;
                if (x == MAX && y == MAX)
                {
                    if (!shortest)
                    {
                        shortest = path;
                    }
                    longest = path.size();
                }
                else if (x >= 0 && x <= MAX && y >= 0 && y <= MAX)
                {
                    horizon.emplace_back(x, y, path);
                }
            }
        }
        // NOLINTEND
    }
    return {*shortest, std::to_string(longest)};
}
} // namespace

template <> struct SolutionType<YEAR, DAY>
{
    using type = aoc::StringSolution;
};

template <> StringSolution solve<YEAR, DAY>(std::istream& input)
{
    std::string seed;
    input >> seed;
    return bfs<4>(seed);
}
} // namespace aoc
