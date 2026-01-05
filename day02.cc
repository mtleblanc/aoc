#include <fstream>
#include <iostream>
#include <cassert>

template <typename T>
T ipow(T base, T exp)
{
    T result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return result;
}

struct sol
{
    uint64_t part1;
    uint64_t part2;

    sol &operator+=(const sol &other)
    {
        part1 += other.part1;
        part2 += other.part2;
        return *this;
    }
};

// will overflow by 16 digits, possibly at 14
sol improved_helper(uint64_t from, uint64_t to, uint64_t digits)
{
    uint64_t lim = ipow(10ull, digits);
    assert(from < to);
    assert(from >= lim / 10);
    assert(to < lim);
    sol accum{0, 0};
    uint64_t pos[]{2, 3, 5, 7, 11, 13};
    uint64_t neg[]{6, 10, 14, 15};
    for (auto it : pos)
    {
        if (digits % it)
            continue;
        uint64_t replength = digits / it;
        uint64_t div = ipow(10ull, digits - replength);
        uint64_t mul = (lim - 1) / (ipow(10ull, replength) - 1);
        uint64_t min = from / div;
        if (min * mul < from)
            min++;
        uint64_t max = to / div;
        if (max * mul > to)
            max--;
        if (max < min)
            continue;
        uint64_t sum = max * (max + 1) / 2 - min * (min - 1) / 2;
        accum.part2 += sum * mul;
        if (it == 2)
            accum.part1 += sum * mul;
    }
    for (auto it : neg)
    {
        if (digits % it)
            continue;
        uint64_t replength = digits / it;
        uint64_t div = ipow(10ull, digits - replength);
        uint64_t mul = (lim - 1) / (ipow(10ull, replength) - 1);
        uint64_t min = from / div;
        if (min * mul < from)
            min++;
        uint64_t max = to / div;
        if (max * mul > to)
            max--;
        if (max < min)
            continue;
        uint64_t sum = max * (max + 1) / 2 - min * (min - 1) / 2;
        accum.part2 -= sum * mul;
    }
    return accum;
}

sol improved(uint64_t from, uint64_t to)
{
    uint64_t digits = 1;
    uint64_t pow = 10;
    sol accum{0, 0};
    while (from < to)
    {
        while (from >= pow)
        {
            pow *= 10;
            digits++;
        }
        accum += improved_helper(from, std::min(pow - 1, to), digits);
        from = pow;
    }
    return accum;
}

uint64_t part1(uint64_t from, uint64_t to)
{
    // could just use log/convert to string and call length
    uint64_t evenDigitsMin = 1;
    uint64_t multiplierMin = 10;
    while (evenDigitsMin * 100 <= from)
    {
        evenDigitsMin *= 100;
        multiplierMin *= 10;
    }
    uint64_t evenDigitsMax = evenDigitsMin * 10;
    uint64_t multiplierMax = multiplierMin;
    while (evenDigitsMax * 100 < to)
    {
        evenDigitsMax *= 100;
        multiplierMax *= 10;
    }
    uint64_t min = from / multiplierMin;
    uint64_t max = to / multiplierMax;
    uint64_t range{0};
    if (max * (multiplierMax + 1) > to)
        max -= 1;
    if (max >= multiplierMax)
        max = multiplierMax - 1;
    if (max < multiplierMin / 10)
        goto skip;
    if (min * (multiplierMin + 1) < from)
        min += 1;
    if (min >= multiplierMin)
        goto skip;
    if (min <= multiplierMin / 10)
        min = multiplierMin / 10;
    // just not dealing with splitting ranges across # of digits since the input doesn't have any ranges spanning 2 orders of magnitude
    // assert(multiplierMin == multiplierMax);
    // std::cout << from << "-" << to << ":" << min * (multiplierMin + 1) << "-" << max * (multiplierMax + 1) << std::endl;
    range = max * (max + 1) / 2 - min * (min - 1) / 2;
    return range * (multiplierMin + 1);
skip:
    return 0;
}

uint64_t part2_brute(uint64_t from, uint64_t to)
{
    // part 2 could work similarly to part one but now range splitting is a thing and we need to do inclusion/exclusion to avoid double counting
    // brute force works in 0.12s so :shrug:
    uint64_t accum{0};
    for (uint64_t cur = from; cur <= to; cur++)
    {
        for (uint64_t mod = 10; mod * mod / 10 < cur; mod *= 10)
        {
            uint64_t rem = cur % mod;
            if (rem * 10 < mod)
                continue;
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
    uint64_t maxmax{0};
    sol accum{0, 0};

    while (fs)
    {
        uint64_t from;
        uint64_t to;
        char c, c1;
        fs >> from >> c >> to >> c1;
        if (c != '-')
            break;
        if (to > maxmax)
            maxmax = to;
        accum += improved(from, to);
    }

    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;

    std::cout << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;

    std::cout << accum.part1 << " " << accum.part2;
}