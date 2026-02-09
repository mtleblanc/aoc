#include "aoc.hh"
#include <algorithm>
#include <sstream>

/* https://adventofcode.com/2016/day/3
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 3;

namespace
{
struct Triangle
{
    int first;
    int second;
    int third;

    bool isValid()
    {
        return std::max({first, second, third}) * 2 < first + second + third;
    }
};

[[maybe_unused]] std::istream& operator>>(std::istream& is, Triangle& t)
{
    is >> t.first >> t.second >> t.third;
    return is;
}

std::vector<Triangle> parseColumns(std::istream& is)
{
    std::vector<Triangle> ret;
    Triangle t1{};
    Triangle t2{};
    Triangle t3{};
    while (is >> t1.first >> t2.first >> t3.first >> t1.second >> t2.second >> t3.second >>
           t1.third >> t2.third >> t3.third)
    {
        ret.push_back(t1);
        ret.push_back(t2);
        ret.push_back(t3);
    }
    return ret;
}

} // namespace

template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    auto inp = slurp(input);
    auto iss = std::istringstream{inp};
    auto part1Tris = readAll<Triangle>(iss);
    auto part1 = std::ranges::count_if(part1Tris, &Triangle::isValid);

    iss = std::istringstream{inp};
    auto part2Tris = parseColumns(iss);
    auto part2 = std::ranges::count_if(part2Tris, &Triangle::isValid);
    return {static_cast<int>(part1), static_cast<int>(part2)};
}
} // namespace aoc
