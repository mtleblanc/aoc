#include "aoc.hh"
#include "util.hh"
#include <cassert>

/* https://adventofcode.com/2017/day/15
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 15;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
using Residue = uint64_t;
constexpr Residue A_FAC = 16807;
constexpr Residue B_FAC = 48271;
constexpr Residue MOD = 2147483647;
constexpr Residue MOD_SHIFT = 31;
constexpr Residue MASK = (1U << 16U) - 1U;
constexpr auto PART1_REPS = 40'000'000;

template <Residue FAC> struct Generator
{
    Residue state;
    Generator(Residue seed) : state{seed} {};
    Residue operator()()
    {
        // Just doing previous = (state * FAC) % MOD, but it seems the compiler's optimization of
        // % (2^31 - 1) is slow
        //
        // compiler-explorer shows gcc computing division by MOD via k / n = (k * [(1<<m)/n]) >> m
        //
        // we can beat that recognizing that 2^31 % MOD = 1, and previous < MOD to start, so we have
        // at most one carry
        //
        // speedup of about 75%
        state *= FAC;
        auto carry = state >> MOD_SHIFT;
        state &= MOD;
        state += carry;
        if (state > MOD)
        {
            state -= MOD;
        }
        return state;
    }
};

auto part1(Residue aSeed, Residue bSeed)
{
    auto a = Generator<A_FAC>{aSeed};
    auto b = Generator<B_FAC>{bSeed};
    int count{};
    for (int i{}; i < PART1_REPS; ++i)
    {
        if (((a() ^ b()) & MASK) == 0)
        {
            ++count;
        }
    }
    return count;
}

constexpr Residue A_MOD = 4;
constexpr Residue B_MOD = 8;
constexpr auto PART2_REPS = 5'000'000;

auto part2(Residue aSeed, Residue bSeed)
{
    auto a = Generator<A_FAC>{aSeed};
    auto b = Generator<B_FAC>{bSeed};
    int count{};
    for (int i{}; i < PART2_REPS; ++i)
    {
        Residue aRes{};
        Residue bRes{};
        while ((aRes = a()) % A_MOD)
        {
        }
        while ((bRes = b()) % B_MOD)
        {
        }
        if (((aRes ^ bRes) & MASK) == 0)
        {
            ++count;
        }
    }
    return count;
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto starts = readNumbers<Residue>(slurp(input));
    assert(starts.size() == 2);
    auto aSeed = starts[0];
    auto bSeed = starts[1];
    return {part1(aSeed, bSeed), part2(aSeed, bSeed)};
}
} // namespace aoc
