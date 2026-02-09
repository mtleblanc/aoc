#include "aoc.hh"
#include <ranges>

/* https://adventofcode.com/2016/day/16
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 16;

namespace
{
void expand(std::string& base)
{
    base.reserve(base.size() * 2 + 1);
    std::string_view orig{base};
    auto tail = orig | std::views::reverse | std::views::transform([](auto c) { return c ^ 1; });
    base.push_back('0');
    base.append_range(tail);
}

// caller is responsible for ensuring str.size() is even.  If odd, last character will be ignored
std::string checksum(const std::string& str)
{
    std::string ret;
    for (size_t i{1}; i < str.size(); i += 2)
    {
        ret.push_back((str[i] == str[i - 1]) ? '1' : '0');
    }
    return ret;
}

template <size_t N> std::string expandAndCheck(std::string data)
{
    while (data.size() < N)
    {
        expand(data);
    }
    data.resize(N);
    while (data.size() % 2 == 0)
    {
        data = checksum(data);
    }
    return data;
}
} // namespace

template <> struct SolutionType<YEAR, DAY>
{
    using type = aoc::StringSolution;
};

template <> StringSolution solve<YEAR, DAY>(std::istream& input)
{
    constexpr auto PART1_DISK = 272UZ;
    constexpr auto PART2_DISK = 35651584UZ;
    std::string seed;
    input >> seed;
    return {expandAndCheck<PART1_DISK>(seed), expandAndCheck<PART2_DISK>(seed)};
}
} // namespace aoc
