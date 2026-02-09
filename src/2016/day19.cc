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
// For exposition, reframe the puzzle as every other player dies repeatedly until 1 is left.  Who is
// doing the killing/stealing is irrelevant.
//
// The idea is to execute one round, and then realize we
// are just playing the game again with roughly half the players. We have a few options for defining
// a round
//
// 1) End the round once each player has died or lived - track whether the next round starts with
// live or die
//
// 2) End the round once half the players have died - track where next round starts
//
// 3) End the round just before the first player lives a second time - every round starts with a
// live
//
// Notably, in all three, the living players all share parity, so we can remember that and then
// right shift all the ids and recurse
//
// Approach 1:
//
// J(n,k) = J((n+1-k)/2, n % 2 ^ k) * 2 + k
//
// where J(n,k) represents the surivor of the n player game starting on player 0 and killing k (0 or
// 1) players.
//
//
// Approach 2, 3:
//
// J(n) = (J(n/2) = n%2)*2
//
// Where J(n) reperesents the survivor of the n player game starting with player 0 living.
[[maybe_unused]] constexpr int josephusKill1(int n)
{
    if (n <= 2)
    {
        return 0;
    }
    return 2 * (josephusKill1(n / 2) + n % 2);
}

// But all that's doing is shifting, except for the base case
// so we don't need the recursion, it's just removing the highest bit.
// ie if f(n) = n with it's highest 1 bit set to 0, J(n,d) = f(n)*2 + d;
[[maybe_unused]] constexpr int josephusBitManip(int n)
{
    auto leadingZeros = std::countl_zero(static_cast<uint32_t>(n));
    // NOLINTNEXTLINE
    n -= 1 << (31 - leadingZeros);
    return n * 2 + 1;
}

// the target moves forward by two spots every other turn due to the circle getting smaller
// renumber the players so that player 1 is the first victim.  we don't need to keep track of who's
// doing the stealing, in fact we can reimagine it as each person stealing from the next two
// so we can do the recursion in base 3 instead of binary.  unfortunately it's not quite as clean
//
// 1) need to special case 4 - with 0,1,2,3 and 0 surviving, we kill 1 and 2, then 3 survives.  to
// recurse we want to wrap to a survivor, but that would kill both 0 and 3 and give the wrong answer
//
// 2) the kill-1 case can reduce to n/2 each time, we remove ceil(n/2) in one round, leaving
// floor(n/2) but with kill-2, we remove 2*ceil(n/3), so we never change parity.
[[maybe_unused]] constexpr int josephusKill2(int n)
{
    if (n == 4)
    {
        return 3;
    }
    if (n < 4)
    {
        return 0;
    }
    auto rec = josephusKill2(n - (n + 2) / 3 * 2) + mod(-n, 3);
    return rec * 3;
}

// represents a starting state with n players, 0 indexed, with the first k being eliminated
// (i.e. k = 0, first player survives, k = 1, first dies, second lives)
//
// I don't have a good explanation for the (n%3 - k > 0); by calculating the 6 cases, (2,0),
// (2,1), and (1,0) need to add 3, the others don't.
[[maybe_unused]] constexpr int josephusKill2Ternary(int n, int k)
{
    if (n == 1)
    {
        return 0;
    }
    if (n <= 3)
    {
        return k;
    }
    auto nextKill = (n - k - n / 3) % 2;
    auto rotation = k + ((n % 3 - k > 0) ? 3 : 0);
    return josephusKill2Ternary(n / 3, nextKill) * 3 + rotation;
}

// translating to the kill-2 version.  perform 1 or 2 steals to get to a survivor and rotate so that
// they are numbered 0
[[maybe_unused]] constexpr int josephusCross(int n)
{
    auto shift = n / 2 + 3 - n % 2;
    auto startSize = n - 2 + n % 2;
    return mod(josephusKill2Ternary(startSize, 0) + shift - 1, n) + 1;
}

// linear time but clear, just need to get the starting point correct then it's just erase 2 skip 1
// until only 1 is left
[[maybe_unused]] constexpr int josephusCrossBrute(int n)
{
    auto living = std::views::iota(1, n + 1) | std::ranges::to<std::list>();
    auto victim = std::next(living.begin(), n / 2);
    for (bool dbl{n % 2 == 1}; std::next(living.begin(), 1) != living.end();)
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
    int n{};
    input >> n;
    return {josephusBitManip(n), josephusCross(n)};
}
} // namespace aoc
