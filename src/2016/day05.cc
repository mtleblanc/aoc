#include "aoc.hh"
#include "hash.hh"
#include <algorithm>
#include <ranges>

/* https://adventofcode.com/2016/day/5
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 5;

namespace
{
template <int N> bool hasStartingZeros(const std::vector<uint8_t>& md5)
{
    if constexpr (N % 2 == 1)
    {
        constexpr auto FIRST_DIGIT = 0xF0;
        if (md5[N / 2] & FIRST_DIGIT)
        {
            return false;
        }
    }
    auto view = md5 | std::views::take(N / 2);
    return std::ranges::all_of(view, [](auto c) { return c == 0; });
}

template <int LEADING_ZEROS> StringSolution crack(const std::string& prefix)
{
    auto hasher = Hash::Hasher::md5Hasher();
    auto appendN = [&prefix](auto n) { return prefix + std::to_string(n); };
    {
        using namespace std::views;
        constexpr auto SECOND_DIGIT = 0x0F;
        constexpr int PASSWORD_LENGTH = 8;
        constexpr int BASE = 16;
        auto view = iota(0) | transform(appendN) |
                    transform([&hasher](const std::string& s) { return hasher(s); }) |
                    filter(hasStartingZeros<LEADING_ZEROS>) |
                    transform(
                        [](auto md5) -> std::pair<int, int>
                        {
                            return std::make_pair(md5[LEADING_ZEROS / 2] & SECOND_DIGIT,
                                                  md5[LEADING_ZEROS / 2 + 1] >> 4);
                        });
        std::bitset<PASSWORD_LENGTH> seen;
        int part1{};
        int part2{};
        for (int i{}; auto [d1, d2] : view)
        {
            if (i++ < PASSWORD_LENGTH)
            {
                part1 = part1 * BASE + d1;
            }

            if (d1 < PASSWORD_LENGTH && !seen[d1])
            {
                seen.set(d1);
                // d1 = 0 means shift 7 nibbles, d1 = 7 means shift 0
                part2 |= d2 << ((PASSWORD_LENGTH - 1 - d1) * 4);
            }

            if (seen.all())
            {
                break;
            }
        }
        return {toHex(part1), toHex(part2)};
    }
}
} // namespace

template <> struct SolutionType<YEAR, DAY>
{
    using type = aoc::StringSolution;
};

template <> StringSolution solve<YEAR, DAY>(std::istream& input)
{
    std::string seed;
    input >> seed;
    // Set to 5 for correct answer, lower so this runs quickly when working on later problems
    constexpr auto ZEROS = 1U;
    return crack<ZEROS>(seed);
}
} // namespace aoc
