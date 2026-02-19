#include "aoc.hh"
#include <algorithm>
#include <ranges>
#include <set>
#include <sstream>

/* https://adventofcode.com/2017/day/4
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 4;
using Solution = Solution_t<YEAR, DAY>;

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    const auto passphrases = readAllLines(input) |
                             std::views::transform(
                                 [](const auto& line)
                                 {
                                     std::stringstream ss{line};
                                     return readAll<std::string>(ss);
                                 }) |
                             std::ranges::to<std::vector>();

    auto allUnique = [](const auto& ps)
#if __cpp_lib_containers_ranges >= 202202L
    { return std::set(std::from_range, ps).size() == ps.size(); };
#else
    { return std::set(ps.begin(), ps.end()).size() == ps.size(); };
#endif

    auto sortElements = [](const auto& r)
    {
        return r | std::views::transform(
                       [](auto e)
                       {
                           std::ranges::sort(e);
                           return e;
                       });
    };

    const auto sortedPassprhases = passphrases | std::views::transform(sortElements);
    return {static_cast<int>(std::ranges::count_if(passphrases, allUnique)),
            static_cast<int>(std::ranges::count_if(sortedPassprhases, allUnique))};
}
} // namespace aoc
