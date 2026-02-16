#include "aoc.hh"
#include "assembunny.hh"

/* https://adventofcode.com/2016/day/23
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 23;

// namespace Assembunny

template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    using namespace Assembunny;
    auto flatProgram = readAll<Instruction>(input);
    constexpr auto PART1_INPUT = 7;
    constexpr auto PART2_INPUT = 12;
    Machine cm{flatProgram};
    cm.reset(PART1_INPUT);
    cm.run();
    auto part1 = cm.a;
    cm = {flatProgram};
    cm.reset(PART2_INPUT);
    cm.run();
    return {part1, cm.a};
}
} // namespace aoc
