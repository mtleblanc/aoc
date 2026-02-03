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
template <size_t N> bool hasStartingZeros(const std::vector<uint8_t>& md5)
{
    if constexpr (N % 2 == 1)
    {
        constexpr uint8_t FIRST_DIGIT = 0xF0;
        if (md5[N / 2] & FIRST_DIGIT)
        {
            return false;
        }
    }
    auto view = md5 | std::views::take(N / 2);
    return std::ranges::all_of(view, [](auto c) { return c == 0; });
}

Solution crack(const std::string& prefix)
{
    auto hasher = Hash::Hasher::md5Hasher();
    auto appendN = [&prefix](auto n) { return prefix + std::to_string(n); };
    {
        using namespace std::views;
        constexpr size_t LEADING_ZEROS = 5;
        constexpr uint8_t SECOND_DIGIT = 0x0F;
        constexpr size_t PASSWORD_LENGTH = 8;
        constexpr size_t BASE = 16;
        auto view = iota(0) | transform(appendN) |
                    transform([&hasher](const std::string& s) { return hasher(s); }) |
                    filter(hasStartingZeros<LEADING_ZEROS>) |
                    transform(
                        [](auto md5) -> std::pair<size_t, size_t>
                        {
                            return std::make_pair(md5[LEADING_ZEROS / 2] & SECOND_DIGIT,
                                                  md5[LEADING_ZEROS / 2 + 1] >> 4);
                        });
        std::bitset<PASSWORD_LENGTH> seen;
        Solution s;
        for (size_t i{}; auto [d1, d2] : view)
        {
            if (i++ < PASSWORD_LENGTH)
            {
                s.part1 = s.part1 * BASE + d1;
            }

            if (d1 < PASSWORD_LENGTH && !seen[d1])
            {
                seen.set(d1);
                // d1 = 0 means shift 7 nibbles, d1 = 7 means shift 0
                s.part2 |= d2 << ((PASSWORD_LENGTH - 1 - d1) * 4);
            }

            if (seen.all())
            {
                break;
            }
        }
        return s;
    }
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    std::string seed;
    input >> seed;
    // NB need to convert answers to hex
    return crack(seed);
}
} // namespace aoc
