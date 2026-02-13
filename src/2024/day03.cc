#include "aoc.hh"
#include <charconv>
#include <regex>

/* https://adventofcode.com/2024/day/3
 *
 * Input:
 * <junk>mul(123,45)<junk>mul(23,456)<junk>...
 *
 * Part 1: Identify mul(x,y) where x and y have 1 to 3 digits.  Multiply all such x*y and sum the
 * results
 * Part 2: Also identify "do" and "don't", and only add a product if "do" was more recent (or none
 * have been seen, i.e. starting state is on)
 */
namespace aoc
{
constexpr size_t YEAR = 2024;
constexpr size_t DAY = 3;

namespace
{
const std::regex rgx(R"((?:mul\((\d{1,3}),(\d{1,3})\))|(do(?:n't)?))");
}
template <> Solution_t<YEAR,DAY> solve<YEAR, DAY>(std::istream& input)
{
    auto in = slurp(input);
    Solution_t<YEAR,DAY> s;
    bool on{true};
    for (auto rit = std::sregex_iterator(in.begin(), in.end(), rgx); rit != std::sregex_iterator();
         ++rit)
    {
        if ((*rit)[3].matched)
        {
            // if matched, it's either "do" or "don't", length discriminates
            on = (*rit)[3].length() == 2;
            continue;
        }
        ssize_t lhs{};
        ssize_t rhs{};
        auto lm = (*rit)[1];
        auto rm = (*rit)[2];
        std::from_chars(&*lm.first, &*lm.second, lhs);
        std::from_chars(&*rm.first, &*rm.second, rhs);
        s.part1 += lhs * rhs;
        if (on)
        {
            s.part2 += lhs * rhs;
        }
    }
    return s;
}
} // namespace aoc
