#include "aoc.hh"
#include <bit>
#include <list>
#include <ranges>

/* https://adventofcode.com/2016/day/19
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 19;

namespace
{
constexpr int josephus(int size)
{
    // assume zero based indices and just add 1 at the end
    //
    // first time around the circle (stopping before player n kills player 1), all players ending in
    // binary digit 1 are killed, remaining players end in 0. relabel shifting right and we are
    // playing josephus again, except we might be starting on die instead of live if there were an
    // odd number of players in the first round this condition is reversed if we killed the first
    // player
    //
    // if J(n, k) is the 0 index based survive, with k = 1 indicating 0 dies first, 0 indicated 0
    // kills 1 first
    //
    // then J(n,k) = J((n+1-k)/2, n % 2 ^ k) * 2 + k
    //
    // that's messy, it would be better if we could just do J(n,k) = f(J(n/2, ?))
    //
    // if instead we loop after the last player kills the first, we can just swap between 0 based
    // and 1 based indexing, call that d, then
    // J(n, d) = J(n/2, n%2)*2 + d with J(1, d) = d
    //
    // But all that's doing is shifting, except for the base case
    // so we don't need the recursion, it's just removing the highest bit.
    // ie if f(n) = n with it's highest 1 bit set to 0, J(n,d) = f(n)*2 + d;

    auto leadingZeros = std::countl_zero(static_cast<uint32_t>(size));
    // NOLINTNEXTLINE
    size -= 1 << (31 - leadingZeros);
    return size * 2 + 1;
}

constexpr int josephusCross(int size)
{
    auto living = std::views::iota(1, size + 1) | std::ranges::to<std::list>();
    auto victim = std::next(living.begin(), size / 2);
    for (bool dbl{size % 2 == 1}; std::next(living.begin(), 1) != living.end();)
    {
        victim = living.erase(victim);
        if (victim == living.end())
        {
            victim = living.begin();
        }
        if (dbl)
        {
            ++victim;
            if (victim == living.end())
            {
                victim = living.begin();
            }
        }
        dbl = !dbl;
    }
    return living.front();
}
} // namespace
template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    int size{};
    input >> size;
    return {josephus(size), josephusCross(size)};
}
} // namespace aoc
