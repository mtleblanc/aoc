#include "aoc.hh"
#include <algorithm>
#include <ranges>

/* https://adventofcode.com/2016/day/18
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 18;

namespace
{
template <int ROWS> int countSafe(std::string row)
{
    if (row.size() < 2)
    {
        return 0;
    }
    auto accum = std::ranges::count(row, '.');
    std::string newRow(row.size(), '.');
    for (auto _ : std::views::iota(0, ROWS - 1))
    {
        newRow[0] = row[1];
        newRow.back() = row[row.size() - 2];
        for (auto i : std::views::iota(1UZ, row.size() - 1))
        {
            newRow[i] = row[i - 1] == row[i + 1] ? '.' : '^';
        }
        std::swap(row, newRow);
        accum += std::ranges::count(row, '.');
    }
    return static_cast<int>(accum);
}
} // namespace

template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    constexpr auto PART1_ROWS = 40;
    constexpr auto PART2_ROWS = 400000;
    std::string row{};
    input >> row;
    return {countSafe<PART1_ROWS>(row), countSafe<PART2_ROWS>(row)};
}
} // namespace aoc
