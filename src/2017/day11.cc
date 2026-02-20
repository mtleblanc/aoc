#include "aoc.hh"
#include "util.hh"
#include <map>
#include <ranges>

/* https://adventofcode.com/2017/day/11
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 11;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
enum class Dir
{
    N,
    NE,
    SE,
    S,
    SW,
    NW
};
const std::map<std::string, Dir> lut = {{"n", Dir::N}, {"ne", Dir::NE}, {"se", Dir::SE},
                                        {"s", Dir::S}, {"sw", Dir::SW}, {"nw", Dir::NW}};

auto parse(std::string_view text)
{
    return text | std::views::split(',') |
           std::views::transform(
               [](auto sv)
               {
                   auto s = std::string{sv.begin(), sv.end()};
                   if (!lut.contains(s))
                   {
                       throw std::invalid_argument("Unknown direction: " + s);
                   }
                   return lut.at(s);
               }) |
           std::ranges::to<std::vector>();
}

auto hexhatten(auto sw, auto se)
{
    // if signs are opposite, this is the best we can do
    // if they are the same, we can do better by replacing sw + se = n
    if (sw < 0)
    {
        sw = -sw;
        se = -se;
    }
    return se > 0 ? std::max(sw, se) : sw - se;
}

auto solve(std::span<Dir> dirs)
{
    auto farthest = int{};
    auto sw = int{};
    auto se = int{};
    for (auto d : dirs)
    {
        switch (d)
        {
        case Dir::N:
            --sw;
            --se;
            break;
        case Dir::NE:
            --sw;
            break;
        case Dir::SE:
            ++se;
            break;
        case Dir::S:
            ++se;
            ++sw;
            break;
        case Dir::SW:
            ++sw;
            break;
        case Dir::NW:
            --se;
            break;
        }
        farthest = std::max(farthest, hexhatten(sw, se));
    }
    return Solution{hexhatten(sw, se), farthest};
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto dirs = parse(trim(slurp(input)));
    return solve(dirs);
}
} // namespace aoc
