#include <fstream>
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <utility>

int main()
{
    constexpr int N = 12;
    std::fstream fs{"input03.txt"};
    uint64_t accum{0};
    std::string s{};
    char maxes[N];
    while (fs >> s)
    {
        if (s.length() < N)
            break;
        // just start at the end and move backwards greedily
        // if maxes were pointers and maxes[-1] were the iterator,
        // we'd have an invariant that maxes[i] points to the leftmost largest element both right of maxes[i-1] and at least N-i from the end
        // but we only need the values, as long as we don't break on equality
        // (eg in xxxxx76, when moving to xxxx776 we have to move maxes[0] to the first 7 to make space for maxes[1] on the second 7)
        auto rit = s.crbegin();
        for (int i = N - 1; i >= 0; i--)
        {
            maxes[i] = *(rit++);
        }
        for (; rit != s.crend(); ++rit)
        {
            char c{*rit};
            for (int i = 0; i < N; i++)
            {
                if (c < maxes[i])
                    break;
                std::swap(c, maxes[i]);
            }
        }
        uint64_t best{0};
        for (int i = 0; i < N; i++)
        {
            best *= 10;
            best += maxes[i] - '0';
        }
        std::cout << best << std::endl;
        accum += best;
    }
    std::cout << accum;
}