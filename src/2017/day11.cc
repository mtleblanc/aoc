#include "aoc.hh"

/* https://adventofcode.com/2017/day/11
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 11;
using Solution = Solution_t<YEAR, DAY>;

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    (void)input;
    return {};
}
} // namespace aoc
