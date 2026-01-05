#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <ranges>

struct range
{
    long min;
    long max;
    bool in(long v)
    {
        return v >= min && v <= max;
    }
    range(std::string s)
    {
        char c;
        std::istringstream ss{s};
        ss >> min >> c >> max;
    }
};

struct event
{
    long id;
    long effect;
    event(long id, long effect) : id{id}, effect{effect} {}
};

uint64_t part2(std::vector<std::string> v)
{
    uint64_t accum {0};
    uint64_t cur {0};
    bool isMult;
    size_t opIdx = v.size() - 1;
    for (size_t x = 0; x < v[0].size(); x++)
    {
        switch (v[opIdx][x])
        {
        case ' ':
            break;
        case '*':
            accum += cur;
            cur = 1;
            isMult = true;
            break;
        case '+':
            accum += cur;
            cur = 0;
            isMult = false;
        }
        uint64_t val {0};
        for(size_t y = 0; y < opIdx; y++) {
            char c = v[y][x];
            if(c != ' ') {
                val *= 10;
                val += c - '0';
            }
        }
        if(val != 0) cur = isMult ? cur * val : cur + val;
    }
    accum += cur;
    return accum;
}

int main()
{
    std::ifstream fs{"input06.txt"};
    std::vector<std::string> v;
    for (std::string s; getline(fs, s);)
    {
        if (s.length() == 0)
            break;
        v.emplace_back(s);
    }

    std::vector<bool> isMult;
    std::vector<uint64_t> results;
    std::istringstream ss{v.back()};
    char c;
    while (ss >> c)
    {
        switch (c)
        {
        case '*':
            isMult.push_back(true);
            results.push_back(1);
            break;
        case '+':
            isMult.push_back(false);
            results.push_back(0);
            break;
        default:
            break;
        }
    }
    for (const auto &l : std::ranges::subrange(v.begin(), v.end() - 1))
    {
        uint64_t cur;
        std::istringstream ss{l};
        size_t i = 0;
        while (ss >> cur)
        {
            results[i] = isMult[i] ? results[i] * cur : results[i] + cur;
            i++;
        }
        assert(i == isMult.size());
    }
    uint64_t accum {};
    for (auto v : results)
        accum += v;
    std::cout << accum << std::endl;
    std::cout << part2(v) << std::endl;
}