#include "aoc.hh"

#include <algorithm>
#include <iostream>
#include <ranges>

/* https://adventofcode.com/2015/day/5
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 5;

namespace
{

bool invalidPair(std::tuple<char, char> p)
{
    auto [a, b] = p;
    constexpr std::string BAD_STARTS = "acpx";
    return BAD_STARTS.contains(a) && (b - a == 1);
}

bool isNicePart1(const std::string_view str)
{
    if (std::ranges::count_if(str,
                              [](auto c)
                              {
                                  constexpr std::string VOWELS = "aeiou";
                                  return VOWELS.contains(c);
                              }) < 3)
    {
        return false;
    }
    auto adjacent = std::views::zip(str, str.substr(1));
    return std::ranges::any_of(adjacent, [](auto p) { return get<0>(p) == get<1>(p); }) &&
           std::ranges::none_of(adjacent, invalidPair);
}

bool containsNonOverlappingPairTwice(std::string_view str)
{
    while (str.size() > 3)
    {
        if (str.substr(2).find(str.substr(0, 2)) != std::string_view::npos)
        {
            return true;
        }
        str = str.substr(1);
    }
    return false;
}

bool isNicePart2(const std::string_view str)
{
    auto paired = std::views::zip(str, str | std::views::drop(2)) |
                  std::views::transform([](auto p) { return get<0>(p) - get<1>(p); });
    bool containsShiftedRepeat = std::ranges::any_of(paired, [](auto c) { return c == 0; });
    return containsShiftedRepeat && containsNonOverlappingPairTwice(str);
}

} // namespace

template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    auto strings = readAllLines(input);

    return {std::ranges::count_if(strings, isNicePart1),
            std::ranges::count_if(strings, isNicePart2)};
}
} // namespace aoc