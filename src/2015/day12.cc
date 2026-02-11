#include "aoc.hh"
#include <nlohmann/json.hpp>

/* https://adventofcode.com/2015/day/12
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 12;

namespace
{

using json = nlohmann::json;
ssize_t part1(const json& data)
{
    if (data.is_number())
    {
        return data.get<ssize_t>();
    }
    if (!data.is_array() && !data.is_object())
    {
        return 0;
    }
    return std::ranges::fold_left(data | std::views::transform(part1), 0L, std::plus<>());
}

ssize_t part2(const json& data)
{

    if (data.is_number())
    {
        return data.get<ssize_t>();
    }
    if (!data.is_array() && !data.is_object())
    {
        return 0;
    }
    if (data.is_object())
    {
        if (std::ranges::find(data, "red") != data.end())
        {
            return 0;
        }
    }

    return std::ranges::fold_left(data | std::views::transform(part2), 0L, std::plus<>());
}

} // namespace

template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    using json = nlohmann::json;
    json data = json::parse(input);
    return {part1(data), part2(data)};
}
} // namespace aoc
