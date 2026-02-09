#include <algorithm>
#include <cassert>
#include <iostream>

#include "aoc.hh"
namespace aoc
{
constexpr size_t YEAR = 2025;
constexpr size_t DAY = 2;
namespace
{
constexpr auto BASE = 10L;
template <typename T> T ipow(T base, T exp)
{
    T result = 1;
    for (;;)
    {
        if (exp & 1)
        {
            result *= base;
        }
        exp >>= 1;
        if (!exp)
        {
            break;
        }
        base *= base;
    }

    return result;
}

// will overflow by 16 digits, possibly at 14
Solution improvedHelper(ssize_t from, ssize_t to, ssize_t digits)
{
    ssize_t lim = ipow(BASE, digits);
    assert(from < to);
    assert(from >= lim / BASE);
    assert(to < lim);
    Solution accum;
    constexpr std::array<ssize_t, 6> POS{2, 3, 5, 7, 11, 13}; // NOLINT(*-magic-numbers)
    constexpr std::array<ssize_t, 4> NEG{6, 10, 14, 15};      // NOLINT(*-magic-numbers)
    for (auto it : POS)
    {
        if (digits % it)
        {
            continue;
        }
        ssize_t replength = digits / it;
        ssize_t div = ipow(BASE, digits - replength);
        ssize_t mul = (lim - 1) / (ipow(BASE, replength) - 1);
        ssize_t min = from / div;
        if (min * mul < from)
        {
            min++;
        }
        ssize_t max = to / div;
        if (max * mul > to)
        {
            max--;
        }
        if (max < min)
        {
            continue;
        }
        ssize_t sum = max * (max + 1) / 2 - min * (min - 1) / 2;
        accum.part2 += sum * mul;
        if (it == 2)
        {
            accum.part1 += sum * mul;
        }
    }
    for (auto it : NEG)
    {
        if (digits % it)
        {
            continue;
        }
        ssize_t replength = digits / it;
        ssize_t div = ipow(BASE, digits - replength);
        ssize_t mul = (lim - 1) / (ipow(BASE, replength) - 1);
        ssize_t min = from / div;
        if (min * mul < from)
        {
            min++;
        }
        ssize_t max = to / div;
        if (max * mul > to)
        {
            max--;
        }
        if (max < min)
        {
            continue;
        }
        ssize_t sum = max * (max + 1) / 2 - min * (min - 1) / 2;
        accum.part2 -= sum * mul;
    }
    return accum;
}

Solution improved(ssize_t from, ssize_t to)
{
    ssize_t digits = 1;
    ssize_t pow = BASE;
    Solution accum;
    while (from < to)
    {
        while (from >= pow)
        {
            pow *= BASE;
            digits++;
        }
        accum += improvedHelper(from, std::min(pow - 1, to), digits);
        from = pow;
    }
    return accum;
}
} // namespace
template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    ssize_t maxmax{};
    Solution accum;

    while (input)
    {
        ssize_t from{};
        ssize_t to{};
        char c{};
        char c1{};
        input >> from >> c >> to >> c1;
        if (c != '-')
        {
            break;
        }
        maxmax = std::max(to, maxmax);
        accum += improved(from, to);
    }

    // auto end = std::chrono::steady_clock::now();
    // auto diff = end - start;

    // std::cout << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;

    return accum;
}
} // namespace aoc