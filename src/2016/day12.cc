#include "aoc.hh"
#include "assembunny.hh"

/* https://adventofcode.com/2016/day/12
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 12;

template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    using namespace Assembunny;
    std::vector<Instruction> program;
    for (Instruction ci; input >> ci;)
    {
        program.push_back(ci);
    }

    Machine cm{program};
    cm.run();
    auto part1 = cm.a;
    cm.reset(0, 0, 1, 0);
    cm.run();

    return {part1, cm.a};
}
} // namespace aoc
