
#include "aoc.hh"

/* https://adventofcode.com/2016/day/9
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 9;

namespace
{
LongSolution decompress(std::istream& input,
                                    int64_t limit = std::numeric_limits<int64_t>::max())
{
    LongSolution sz{};
    auto before = input.tellg();
    for (char c{}; input.tellg() - before < limit && input >> c;)
    {
        if (c != '(')
        {
            ++sz.part1;
            ++sz.part2;
            continue;
        }

        int64_t len{};
        int rep{};
        char c2{};
        if (!(input >> len >> c >> rep >> c2) || c != 'x' || c2 != ')')
        {
            throw std::invalid_argument("bad marker");
        }
        sz.part2 += rep * decompress(input, len).part2;
        sz.part1 += len * rep;
    }
    if (input.tellg() - before > limit)
    {
        throw std::invalid_argument("nested repeat overruns outer repeat length");
    }
    return sz;
}
} // namespace

template <> struct SolutionType<YEAR, DAY>
{
    using type = LongSolution;
};

template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    return decompress(input);
}
} // namespace aoc
