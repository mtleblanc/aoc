#include "aoc.hh"
#include "util.hh"
#include <algorithm>
#include <ctre.hpp>
#include <ctre/wrapper.hpp>
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
template <int LEN> std::vector<int> hash(std::span<int> lengths, int rounds = 1)
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
            pos += l + skip;
            ++skip;
        }
    }
    return string;
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    static constexpr auto PAT = ctll::fixed_string(R"((\d+))");
    static constexpr auto LEN = 256;
    static constexpr auto ROUNDS = 64;
    static constexpr auto CHUNK = 16;
    static constexpr auto SUFFIX = std::array<int, 5>{{17, 31, 73, 47, 23}};
    auto text = slurp(input);
    auto parse = [](auto n) { return n.template get<1>().to_number(); };
    auto part1Lengths =
        ctre::search_all<PAT>(text) | std::views::transform(parse) | std::ranges::to<std::vector>();
    auto part1Hash = hash<LEN>(part1Lengths);
    auto part1 = part1Hash[0] * part1Hash[1];

    auto part2Lengths = trim(text) | std::views::transform([](auto c) -> int { return c; }) |
                        std::ranges::to<std::vector>();
    std::ranges::copy(SUFFIX, std::back_inserter(part2Lengths));
    auto part2Hash = hash<LEN>(part2Lengths, ROUNDS);
    auto part2Dense =
        part2Hash | std::views::chunk(CHUNK) |
        std::views::transform([](const auto& r)
                              { return std::ranges::fold_left(r, 0, std::bit_xor<>()); });
    std::string part2;
    for (auto h : part2Dense)
    {
        part2 += toHex(h);
    }
    return {std::to_string(part1), part2};
}
} // namespace aoc
