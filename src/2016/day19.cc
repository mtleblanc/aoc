#include "aoc.hh"
#include "util.hh"
#include <bit>
#include <cassert>
#include <iostream>
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

// the target moves forward by two spots every other turn due to the circle getting smaller
// renumber the players so that player 1 is the first victim.  we don't need to keep track of who's
// doing the stealing, in fact we can reimagine it as each person stealing from the next two
// so we can do the recursion in base 3 instead of binary.  unfortunately it's not quite as clean
//
// 1) need to special case 4 - with 0,1,2,3 and 0 surviving, we kill 1 and 2, then 3 survives.  to
// recurse we want to wrap to a survivor, but that would kill both 0 and 3 and give the wrong answer
//
// 2) the kill-1 case can reduce to size/2 each time, we remove ceil(size/2) in one round, leaving
// floor(size/2) but with kill-2, we remove 2*ceil(size/3), so we never change parity.  Perhaps we
// can do a recursion with stopping at size/3 but possibly not at a survivor, it would be nice if
// there's a nice ternary solution
constexpr int josephusKill2(int size)
{
    if (size == 4)
    {
        return 3;
    }
    if (size < 4)
    {
        return 0;
    }
    auto rec = josephusKill2(size - (size + 2) / 3 * 2) + mod(-size, 3);
    return rec * 3;
}

// translating to the kill-2 version.  perform 1 or 2 steals to get to a survivor and rotate so that
// they are numbered 0
[[maybe_unused]] constexpr int josephusCross(int size)
{
    auto shift = size / 2 + 3 - size % 2;
    auto startSize = size - 2 + size % 2;
    return mod(josephusKill2(startSize) + shift - 1, size) + 1;
}

// linear time but clear, just need to get the starting point correct then it's just erase 2 skip 1
// until only 1 is left
[[maybe_unused]] constexpr int josephusCrossBrute(int size)
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
    // For testing
    // for (int i = 3; i < 1000; ++i)
    // {
    //     assert(josephusCrossBrute(i) == josephusCross(i));
    // }
    // for(int i = 1; i < 28;++i ) {
    //     std::cout << i << ":" << josephusKill2(i) << std::endl;
    // }
    int size{};
    input >> size;
    return {josephus(size), josephusCross(size)};
}
} // namespace aoc
