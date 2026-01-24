#include "aoc.hh"

/* https://adventofcode.com/2015/day/11
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 11;

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    // if there isn't a pair or triple at the start of the password, the next valid
    // will look like XXXaabcc.  This repo isn't setup to return non-size_t and the problem is
    // solvable by inspection even without pen & paper.
    (void)input;
    return {};
}
} // namespace aoc
