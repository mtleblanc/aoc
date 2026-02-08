#include "aoc.hh"
#include "hash.hh"
#include <algorithm>
#include <deque>
#include <optional>
#include <ranges>

/* https://adventofcode.com/2016/day/14
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 14;

namespace
{
using Digest = Hash::Hasher::Digest;

std::string toString(const Digest& digest)
{
    static constexpr std::string_view DIGITS = "0123456789abcdef";
    constexpr auto WIDTH = 4U;
    constexpr auto SECOND_DIGIT = 0x0F;
    std::string res;
    res.reserve(digest.size() * 2);
    for (auto h : digest)
    {
        res.push_back(DIGITS[h >> WIDTH]);
        res.push_back(DIGITS[h & SECOND_DIGIT]);
    }
    return res;
}

template <size_t REP> std::optional<char> findRepeat(const Digest& digest)
{
    char c{};
    size_t cnt{};
    for (auto d : toString(digest))
    {
        if (d == c)
        {
            ++cnt;
            if (cnt == REP)
            {
                return c;
            }
        }
        else
        {
            c = d;
            cnt = 1;
        }
    }
    return {};
}

template <size_t REP> auto hasRepeat(char c)
{
    return [c](const Digest& digest)
    {
        size_t cnt{};
        for (auto d : toString(digest))
        {
            if (d == c)
            {
                ++cnt;
                if (cnt == REP)
                {
                    return true;
                }
            }
            else
            {
                cnt = 0;
            }
        }
        return false;
    };
}

template <typename F> size_t generateOTPs(const std::string& seed, F& f)
{
    constexpr auto WINDOW = 1000UL;
    constexpr auto TARGET = 64UL;
    constexpr auto FIRST_REPEATS = 3;
    constexpr auto SECOND_REPEATS = 5;
    auto appendN = [&seed](auto n) { return seed + std::to_string(n); };
    std::deque<Digest> hashes;
    size_t otps{};
    for (size_t n{}; hashes.size() < WINDOW;)
    {
        hashes.push_back(f(appendN(n++)));
    }
    for (size_t idx{}; otps < TARGET; ++idx)
    {
        auto h = hashes.front();
        hashes.pop_front();
        hashes.push_back(f(appendN(idx + WINDOW)));
        auto tpl = findRepeat<FIRST_REPEATS>(h);
        if (!tpl)
        {
            continue;
        }
        if (std::ranges::any_of(hashes, hasRepeat<SECOND_REPEATS>(*tpl)))
        {
            ++otps;
            if (otps == TARGET)
            {
                return idx;
            }
        }
    }
    return 0;
}
} // namespace
template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    std::string seed;
    input >> seed;
    auto hasher = Hash::Hasher::md5Hasher();
    auto stretchedHasher = [&hasher](std::string message)
    {
        constexpr auto STRETCH = 2016UL;
        auto hash = hasher(message);
        for (auto _ : std::views::iota(0UL, STRETCH))
        {
            message = toString(hash);
            hash = hasher(message);
        }
        return hash;
    };
    return {generateOTPs(seed, hasher), generateOTPs(seed, stretchedHasher)};
}
} // namespace aoc
