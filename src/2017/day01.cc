#include "aoc.hh"
#include "util.hh"
#include <algorithm>
#include <ranges>

/* https://adventofcode.com/2017/day/1
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 1;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
auto part1(std::string_view in)
{
    std::string captcha{in};
    captcha.push_back(captcha.front());
    auto part1View = std::views::zip(captcha, captcha | std::views::drop(1)) |
                     std::views::filter(
                         [](auto p)
                         {
                             auto [a, b] = p;
                             return a == b;
                         }) |
                     std::views::transform(
                         [](auto p)
                         {
                             auto [a, _] = p;
                             return a - '0';
                         });
    return std::ranges::fold_left(part1View, 0, std::plus<>());
}

auto part2(std::string_view in)
{
    std::string captcha{in};
    auto rep = captcha + captcha;
    auto part1View = std::views::zip(captcha, rep | std::views::drop(captcha.size() / 2)) |
                     std::views::filter(
                         [](auto p)
                         {
                             auto [a, b] = p;
                             return a == b;
                         }) |
                     std::views::transform(
                         [](auto p)
                         {
                             auto [a, _] = p;
                             return a - '0';
                         });
    return std::ranges::fold_left(part1View, 0, std::plus<>());
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto captcha = std::string(trim(slurp(input)));

    return {part1(captcha), part2(captcha)};
}
} // namespace aoc
