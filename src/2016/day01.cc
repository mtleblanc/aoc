#include "aoc.hh"
#include <complex>
#include <ctre.hpp>
#include <iostream>
#include <ranges>
#include <unordered_set>

/* https://adventofcode.com/2016/day/1
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 1;

namespace
{
struct ComplexHash
{
    std::size_t operator()(const std::complex<int>& z) const noexcept
    {
        // NOLINTNEXTLINE
        return z.real() ^ (z.imag() * 37);
    }
};

int taxicab(std::complex<int> z)
{
    return std::abs(z.imag()) + std::abs(z.real());
}
} // namespace
template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    static constexpr auto PAT = ctll::fixed_string(R"(([LR])(\d+))");

    using namespace std::complex_literals;

    constexpr std::complex<int> L = {0, 1};
    constexpr std::complex<int> R = {0, -1};

    auto dirs = slurp(input);
    std::complex<int> loc;
    std::complex<int> direction = {0, 1};
    std::optional<std::complex<int>> firstRepeat;
    std::unordered_set<std::complex<int>, ComplexHash> seen;
    for (auto match : ctre::search_all<PAT>(dirs))
    {
        char dir = match.get<1>().to_view()[0];
        if (dir == 'L')
        {
            direction *= L;
        }
        else
        {
            direction *= R;
        }
        auto steps = std::stoi(std::string(match.get<2>().to_view()));
        for (auto _ : std::views::iota(0, steps))
        {
            loc += direction;
            if (!firstRepeat)
            {
                if (seen.contains(loc))
                {
                    firstRepeat = loc;
                }
                else
                {
                    seen.insert(loc);
                }
            }
        }
    }

    return {taxicab(loc), taxicab(*firstRepeat)};
}
} // namespace aoc
