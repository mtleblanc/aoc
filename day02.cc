#include <fstream>
#include <iostream>
#include <cassert>
#include <vector>

int main()
{
    std::fstream fs{"input02.txt"};
    uint64_t accum{0};
    uint64_t maxmax{0};
    uint64_t accum2{0};

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
        assert(multiplierMin == multiplierMax);
        // std::cout << from << "-" << to << ":" << min * (multiplierMin + 1) << "-" << max * (multiplierMax + 1) << std::endl;
        range = max * (max + 1) / 2 - min * (min - 1) / 2;
        accum += range * (multiplierMin + 1);
    skip:

        // part 2 could work similarly to part one but now range splitting is a thing and we need to do inclusion/exclusion to avoid double counting
        // brute force works in 0.12s so :shrug:
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
                        accum2 += cur;
                        // std::cout << cur << std::endl;
                        goto end;
                    }
                }
            }
        end:
            continue;
        }
    }
    assert(!fs);
    std::cout << accum << " " << accum2;
}