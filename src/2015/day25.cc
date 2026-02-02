#include "aoc.hh"
#include <scn/scan.h>

/* https://adventofcode.com/2015/day/25
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 25;
namespace
{
constexpr size_t SEED = 20151125;
constexpr size_t MUL = 252533;
constexpr size_t MOD = 33554393;

size_t index(size_t row, size_t col)
{
    auto diag = row + col - 2;
    auto start = diag * (diag + 1) / 2;
    return start + col - 1;
}

size_t pmod(size_t n)
{
    size_t res{1};
    size_t mul{MUL};
    while (n)
    {
        if (n % 2)
        {
            res = (res * mul) % MOD;
        }
        mul = (mul * mul) % MOD;
        n /= 2;
    }
    return res;
}
} // namespace
template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    std::string in;
    std::getline(input, in);
    auto [row, col] =
        scn::scan<size_t, size_t>(in, "To continue, please consult the code grid in the manual.  "
                                      "Enter the code at row {}, column {}.")
            ->values();
    return {SEED * pmod(index(row, col)) % MOD};
}
} // namespace aoc
