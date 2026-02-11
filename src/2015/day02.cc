#include <algorithm>

#include "aoc.hh"

/* https://adventofcode.com/2015/day/2
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 2;

template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    char c{};
    ssize_t l{};
    ssize_t w{};
    ssize_t h{};
    ssize_t paper{};
    ssize_t ribbon{};
    while (input >> l >> c >> w >> c >> h)
    {
        auto longest = std::max({l, w, h});
        paper += 2 * (l * w + l * h + w * h) + l * w * h / longest;
        ribbon += l * w * h + 2 * (l + w + h - longest);
    }
    return {paper, ribbon};
}
} // namespace aoc
