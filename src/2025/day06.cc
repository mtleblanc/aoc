#include <cassert>
#include <iostream>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "aoc.hh"
namespace aoc
{
constexpr size_t YEAR = 2025;
constexpr size_t DAY = 6;
namespace
{

ssize_t part2(std::vector<std::string> v)
{
    ssize_t accum{0};
    ssize_t cur{0};
    bool isMult = false;
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
            break;
        default:
        {
            throw std::invalid_argument("Unexpected symbol in input");
        }
        }
        ssize_t val{0};
        constexpr ssize_t BASE = 10;
        for (size_t y = 0; y < opIdx; y++)
        {
            char c = v[y][x];
            if (c != ' ')
            {
                val *= BASE;
                val += c - '0';
            }
        }
        if (val != 0)
        {
            cur = isMult ? cur * val : cur + val;
        }
    }
    accum += cur;
    return accum;
}
} // namespace
template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto lines = readAllLines(input);

    std::vector<bool> isMult;
    std::vector<ssize_t> results;
    std::istringstream ss{lines.back()};
    char c = 0;
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
    for (const auto& l : std::ranges::subrange(lines.begin(), lines.end() - 1))
    {
        ssize_t cur = 0;
        std::istringstream ss{l};
        size_t i = 0;
        while (ss >> cur)
        {
            results[i] = isMult[i] ? results[i] * cur : results[i] + cur;
            ++i;
        }
        assert(i == isMult.size());
    }
    ssize_t accum{};
    for (auto v : results)
    {
        accum += v;
    }
    return Solution{accum, part2(lines)};
}
} // namespace aoc