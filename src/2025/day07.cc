#include <cassert>
#include <ranges>
#include <string>
#include <vector>

#include "aoc.hh"
namespace aoc
{
constexpr size_t YEAR = 2025;
constexpr size_t DAY = 7;
namespace
{
uint64_t part1(const std::vector<std::string>& v)
{
    uint64_t splits{0};
    std::string oldRow{v[0]};
    for (const auto& cur : v | std::views::drop(1))
    {
        std::string newRow{cur};
        for (size_t x = 0; x < v[0].size(); x++)
        {
            if (oldRow[x] != 'S')
            {
                continue;
            }
            if (cur[x] == '.')
            {
                newRow[x] = 'S';
            }
            else
            {
                splits++;
                if (x > 0)
                {
                    newRow[x - 1] = 'S';
                }
                if (x < cur.size() - 1)
                {
                    newRow[x + 1] = 'S';
                }
            }
        }
        oldRow = std::move(newRow);
    }
    return splits;
}

uint64_t part2(const std::vector<std::string>& v)
{
    std::vector<uint64_t> oldRow(v[0].size());
    size_t start = v[0].find('S');
    oldRow[start] = 1;
    for (const auto& cur : v | std::views::drop(1))
    {
        std::vector<uint64_t> newRow(oldRow.size());
        for (size_t x = 0; x < v[0].size(); x++)
        {
            if (cur[x] == '.')
            {
                newRow[x] += oldRow[x];
            }
            else
            {
                if (x > 0)
                {
                    newRow[x - 1] += oldRow[x];
                }
                if (x < cur.size() - 1)
                {
                    newRow[x + 1] += oldRow[x];
                }
            }
        }
        oldRow = std::move(newRow);
    }
    uint64_t accum{0};
    for (auto v : oldRow)
    {
        accum += v;
    }
    return accum;
}

} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto lines = readAllLines(input);
    return Solution{part1(lines), part2(lines)};
}
} // namespace aoc