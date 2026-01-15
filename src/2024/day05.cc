#include "aoc.hh"

/* https://adventofcode.com/2024/day/5
 *
 * Input:
 * 47|53
 * 97|13
 * ...
 *
 * 75,47,61,53,29
 * ...
 *
 * Part 1: The first section of input lists ordering requirements.  Count how many sequences in the
 * second section satisfy all requirements where both numbers appear
 * Part 2: Sort the incorrect sequences and sum their middle numbers
 */
namespace aoc
{
constexpr size_t YEAR = 2024;
constexpr size_t DAY = 5;

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    (void)input;
    return {};
}
} // namespace aoc
