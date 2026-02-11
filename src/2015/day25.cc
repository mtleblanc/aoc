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
constexpr ssize_t SEED = 20151125;
constexpr ssize_t MUL = 252533;
constexpr ssize_t MOD = 33554393;

ssize_t index(ssize_t row, ssize_t col)
{
    auto diag = row + col - 2;
    auto start = diag * (diag + 1) / 2;
    return start + col - 1;
}

ssize_t pmod(ssize_t n)
{
    ssize_t res{1};
    ssize_t mul{MUL};
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
template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    std::string in;
    std::getline(input, in);
    auto [row, col] =
        scn::scan<ssize_t, ssize_t>(in, "To continue, please consult the code grid in the manual.  "
                                        "Enter the code at row {}, column {}.")
            ->values();
    return {SEED * pmod(index(row, col)) % MOD};
}
} // namespace aoc
