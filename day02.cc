#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>

constexpr auto BASE = 10ULL;
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

struct Sol
{
    uint64_t part1{};
    uint64_t part2{};

    Sol& operator+=(const Sol& other)
    {
        part1 += other.part1;
        part2 += other.part2;
        return *this;
    }
};

// will overflow by 16 digits, possibly at 14
Sol improvedHelper(uint64_t from, uint64_t to, uint64_t digits)
{
    uint64_t lim = ipow(BASE, digits);
    assert(from < to);
    assert(from >= lim / BASE);
    assert(to < lim);
    Sol accum;
    constexpr std::array<uint64_t, 6> POS{2, 3, 5, 7, 11, 13}; // NOLINT(*-magic-numbers)
    constexpr std::array<uint64_t, 4> NEG{6, 10, 14, 15};      // NOLINT(*-magic-numbers)
    for (auto it : POS)
    {
        if (digits % it)
        {
            continue;
        }
        uint64_t replength = digits / it;
        uint64_t div = ipow(BASE, digits - replength);
        uint64_t mul = (lim - 1) / (ipow(BASE, replength) - 1);
        uint64_t min = from / div;
        if (min * mul < from)
        {
            min++;
        }
        uint64_t max = to / div;
        if (max * mul > to)
        {
            max--;
        }
        if (max < min)
        {
            continue;
        }
        uint64_t sum = max * (max + 1) / 2 - min * (min - 1) / 2;
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
        uint64_t replength = digits / it;
        uint64_t div = ipow(BASE, digits - replength);
        uint64_t mul = (lim - 1) / (ipow(BASE, replength) - 1);
        uint64_t min = from / div;
        if (min * mul < from)
        {
            min++;
        }
        uint64_t max = to / div;
        if (max * mul > to)
        {
            max--;
        }
        if (max < min)
        {
            continue;
        }
        uint64_t sum = max * (max + 1) / 2 - min * (min - 1) / 2;
        accum.part2 -= sum * mul;
    }
    return accum;
}

Sol improved(uint64_t from, uint64_t to)
{
    uint64_t digits = 1;
    uint64_t pow = BASE;
    Sol accum;
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

uint64_t part1(uint64_t from, uint64_t to)
{
    constexpr auto BASESQ = BASE * BASE;

    // could just use log/convert to string and call length
    uint64_t evenDigitsMin = 1;
    uint64_t multiplierMin = BASE;
    while (evenDigitsMin * BASESQ <= from)
    {
        evenDigitsMin *= BASESQ;
        multiplierMin *= BASE;
    }
    uint64_t evenDigitsMax = evenDigitsMin * BASE;
    uint64_t multiplierMax = multiplierMin;
    while (evenDigitsMax * BASESQ < to)
    {
        evenDigitsMax *= BASESQ;
        multiplierMax *= BASE;
    }
    uint64_t min = from / multiplierMin;
    uint64_t max = to / multiplierMax;
    uint64_t range{};
    if (max * (multiplierMax + 1) > to)
    {
        max -= 1;
    }
    if (max >= multiplierMax)
    {
        max = multiplierMax - 1;
    }
    if (max < multiplierMin / BASE)
    {
        return 0;
    }
    if (min * (multiplierMin + 1) < from)
    {
        min += 1;
    }
    if (min >= multiplierMin)
    {
        return 0;
    }
    min = std::max(min, multiplierMin / BASE);
    // just not dealing with splitting ranges across # of digits since the input doesn't have any
    // ranges spanning 2 orders of magnitude
    //
    // assert(multiplierMin == multiplierMax);
    // std::cout << from << "-" << to << ":" << min * (multiplierMin + 1) << "-" << max *
    // (multiplierMax + 1) << std::endl;
    range = max * (max + 1) / 2 - min * (min - 1) / 2;
    return range * (multiplierMin + 1);
}

uint64_t part2Brute(uint64_t from, uint64_t to)
{
    // part 2 could work similarly to part one but now range splitting is a thing and we need to do
    // inclusion/exclusion to avoid double counting brute force works in 0.12s so :shrug:
    uint64_t accum{};
    for (uint64_t cur = from; cur <= to; cur++)
    {
        for (uint64_t mod = BASE; mod * mod / BASE < cur; mod *= BASE)
        {
            uint64_t rem = cur % mod;
            if (rem * BASE < mod)
            {
                continue;
            }
            uint64_t rep = rem;
            while (rep < cur)
            {
                rep *= mod;
                rep += rem;
                if (rep == cur)
                {
                    accum += cur;
                    // std::cout << cur << std::endl;
                    goto end;
                }
            }
        }
    end:
        continue;
    }
    return accum;
}

int main()
{

    auto start = std::chrono::steady_clock::now();

    std::fstream fs{"challenge02.txt"};
    uint64_t maxmax{};
    Sol accum;

    while (fs)
    {
        uint64_t from{};
        uint64_t to{};
        char c{};
        char c1{};
        fs >> from >> c >> to >> c1;
        if (c != '-')
        {
            break;
        }
        maxmax = std::max(to, maxmax);
        accum += improved(from, to);
    }

    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;

    std::cout << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;

    std::cout << accum.part1 << " " << accum.part2;
}