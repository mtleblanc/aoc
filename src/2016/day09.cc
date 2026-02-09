
#include "aoc.hh"

/* https://adventofcode.com/2016/day/9
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 9;

namespace
{
GeneralSolution<int64_t> decompress(std::istream& input,
                                    ssize_t limit = std::numeric_limits<ssize_t>::max())
{
    GeneralSolution<int64_t> sz{};
    auto before = input.tellg();
    for (char c{}; input.tellg() - before < limit && input >> c;)
    {
        if (c != '(')
        {
            ++sz.part1;
            ++sz.part2;
            continue;
        }

        ssize_t len{};
        ssize_t rep{};
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
    using type = GeneralSolution<int64_t>;
};

template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    return decompress(input);
}
} // namespace aoc
