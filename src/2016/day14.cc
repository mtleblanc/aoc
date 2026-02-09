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

constexpr auto WIDTH = 4U;
constexpr auto SECOND_DIGIT = 0x0F;
constexpr auto LEN = 16UL;

auto splitDigits(const auto& digest)
{
    int idx{};
    std::array<uint8_t, LEN * 2> res{};
    for (auto h : digest)
    {
        res[idx++] = h >> WIDTH;
        res[idx++] = h & SECOND_DIGIT;
    }
    return res;
}

// We need to do 2017 * 20k+ hashes, allocating a string here costs 3 seconds of runtime.  No
// observed difference with passing out parameter/not initializing
auto toHexChars(const auto& digest)
{
    static constexpr std::string_view DIGITS = "0123456789abcdef";
    std::array<char, LEN * 2> res{};
    std::ranges::copy(splitDigits(digest) | std::views::transform([](auto c) { return DIGITS[c]; }),
                      res.begin());
    return res;
}

// using splitDigits instead of unrolling the nibbles is marginally slower but cleaner
template <int REP> std::optional<uint8_t> findRepeat(const auto& digest)
{
    uint8_t c{};
    int cnt{};
    for (auto d : splitDigits(digest))
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

template <int REP> auto findRepeats(const auto& digest)
{
    uint16_t repeats{};
    uint8_t c{};
    int cnt{};
    for (auto d : splitDigits(digest))
    {
        if (d == c)
        {
            ++cnt;
            if (cnt == REP)
            {
                repeats |= 1 << c;
            }
        }
        else
        {
            c = d;
            cnt = 1;
        }
    }
    return repeats;
}

template <typename F> int generateOTPs(const std::string& seed, F& f)
{
    using Digest = decltype(f(std::declval<std::string>()));

    constexpr auto WINDOW = 1000UL;
    constexpr auto TARGET = 64;
    constexpr auto FIRST_REPEATS = 3;
    constexpr auto SECOND_REPEATS = 5;

    auto appendN = [&seed](auto n) { return seed + std::to_string(n); };
    std::deque<Digest> hashes;
    std::deque<uint16_t> repeats;
    int otps{};
    for (int n{}; hashes.size() < WINDOW;)
    {
        hashes.push_back(f(appendN(n++)));
        repeats.push_back(findRepeats<SECOND_REPEATS>(hashes.back()));
    }
    for (int idx{}; otps < TARGET; ++idx)
    {
        auto h = hashes.front();
        hashes.pop_front();
        repeats.pop_front();
        hashes.push_back(f(appendN(idx + WINDOW)));
        repeats.push_back(findRepeats<SECOND_REPEATS>(hashes.back()));
        auto tpl = findRepeat<FIRST_REPEATS>(h);
        if (!tpl)
        {
            continue;
        }
        if (std::ranges::any_of(repeats, [&tpl](const auto& s) { return s & 1 << *tpl; }))
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
template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    std::string seed;
    input >> seed;
    auto hasher = Hash::Hasher::md5Hasher();
    auto stretchedHasher = [&hasher](const std::string& message)
    {
        // Set to 2016 for correct answer.  Reduced for runtime while solving other problems
        constexpr auto STRETCH = 0;
        constexpr auto LEN = 16UL;
        std::array<char, LEN * 2> str{};
        auto hash = hasher.digest<LEN>(message);
        for (auto _ : std::views::iota(0, STRETCH))
        {
            str = toHexChars(hash);
            hash = hasher.digest<LEN>(std::string_view{str.begin(), str.end()});
        }
        return hash;
    };
    return {generateOTPs(seed, hasher), generateOTPs(seed, stretchedHasher)};
}
} // namespace aoc
