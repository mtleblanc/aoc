#include "aoc.hh"
#include "util.hh"
#include <bit>
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
[[maybe_unused]] constexpr int josephus(int size)
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
// doing the stealing.  Say we start with the double advance.  So player 1 is out, then 3, then 4,
// then 6, then 7, etc, every 3rd player survives.  We can do the recursion in base 3
constexpr int josephusKill2(int size, int remainder)
{
    constexpr std::array<int, 6> BASE = {{0, 0, 0, 0, 3, 3}};
    if (size < std::ssize(BASE))
    {
        return remainder + BASE[size];
    }
    auto rec = josephusKill2(size - (size + 2) / 3 * 2, mod(-size, 3));
    return rec * 3 + remainder;
}

[[maybe_unused]] constexpr int josephusCross(int size)
{
    return mod(josephusKill2(size - 2 + size % 2, size / 2 + 3 - size % 2) - 1, size) + 1;
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
    // for (int i = 23; i < 36; ++i)
    // {
    //     std::cout << i << ": " << (josephusCrossBrute(i) + i - i / 2 - 1) % i + 1 << " ("
    //               << josephusCrossBrute(i) << " - " << josephusCross(i) << ")" << '\n';
    // }
    int size{};
    input >> size;
    return {josephus(size), josephusCross(size)};
}
} // namespace aoc
