#include "aoc.hh"

/* https://adventofcode.com/2018/day/13
 */
namespace aoc
{
constexpr size_t YEAR = 2018;
constexpr size_t DAY = 13;
using Solution = Solution_t<YEAR, DAY>;

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    (void)input;
    return {};
}
} // namespace aoc
