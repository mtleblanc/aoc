#include "aoc.hh"
#include "util.hh"
#include <algorithm>
#include <ctre.hpp>
#include <functional>
#include <iostream>
#include <ranges>

/* https://adventofcode.com/2017/day/10
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 10;
template <> struct SolutionType<YEAR, DAY>
{
    using type = StringSolution;
};
using Solution = Solution_t<YEAR, DAY>;
namespace
{
constexpr auto LEN = 256;
std::vector<int> hash(std::span<int> lengths, int rounds = 1)
{
    int pos{};
    int skip{};
    auto string = std::views::iota(0, LEN) | std::ranges::to<std::vector>();
    auto repView = std::views::iota(0UZ) |
                   std::views::transform([&string](auto n) -> auto& { return string[n % LEN]; });
    for ([[maybe_unused]] auto _ : std::views::iota(0, rounds))
    {
        for (auto l : lengths)
        {
            auto start = std::ranges::next(repView.begin(), pos);
            std::ranges::reverse(start, std::ranges::next(start, l));
            pos += l + skip++;
        }
    }
    return string;
}

int part1(std::string_view text)
{

    constexpr auto PAT = ctll::fixed_string(R"((\d+))");
    auto parse = [](auto n) { return n.template get<1>().to_number(); };
    auto part1Lengths =
        ctre::search_all<PAT>(text) | std::views::transform(parse) | std::ranges::to<std::vector>();
    auto part1Hash = hash(part1Lengths);
    return part1Hash[0] * part1Hash[1];
}

std::string part2(std::string_view text)
{
    static constexpr auto ROUNDS = 64;
    static constexpr auto CHUNK = 16;
    static constexpr auto SUFFIX = std::array<int, 5>{{17, 31, 73, 47, 23}};
    auto lengths = trim(text) | std::views::transform([](auto c) -> int { return c; }) |
                   std::ranges::to<std::vector>();
    std::ranges::copy(SUFFIX, std::back_inserter(lengths));
    auto dense =
        hash(lengths, ROUNDS) | std::views::chunk(CHUNK) |
        std::views::transform([](auto r)
                              { return toHex(std::ranges::fold_left(r, 0, std::bit_xor<>()), 2); });
    return std::ranges::fold_left(dense, std::string{}, std::plus<>());
}

} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto text = slurp(input);

    return {std::to_string(part1(text)), part2(text)};
}
} // namespace aoc
