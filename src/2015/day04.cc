#include "aoc.hh"
#include "hash.hh"
#include <algorithm>
#include <ranges>
#include <thread>

/* https://adventofcode.com/2015/day/4
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 4;

namespace
{
template <ssize_t N> bool hasStartingZeros(const std::vector<uint8_t>& md5)
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

template <ssize_t N>
std::optional<ssize_t> mineSlice(const std::string& prefix, ssize_t from, ssize_t count,
                                Hash::Hasher& hasher)
{
    auto appendN = [&prefix](auto n) { return prefix + std::to_string(n); };

    auto view = std::views::iota(from, from + count) | std::views::transform(appendN) |
                std::views::transform([&hasher](const std::string& s) { return hasher(s); });
    auto found = std::ranges::find_if(view, hasStartingZeros<N>);
    return found == view.end() ? std::optional<ssize_t>{} : found - view.begin() + from;
}

template <ssize_t N, ssize_t BLOCKSIZE>
// NOLINTNEXTLINE (performance-unnecessary-value-param)
void mine(std::string prefix, std::atomic<ssize_t>& counter, std::atomic<ssize_t>& result)
{
    auto hasher = Hash::Hasher::md5Hasher();
    for (;;)
    {
        if (result.load() != 0)
        {
            break;
        }
        ssize_t from = counter.fetch_add(BLOCKSIZE);
        auto res = mineSlice<N>(prefix, from, BLOCKSIZE, hasher);
        if (res)
        {
            ssize_t prev{};
            do
            {
                prev = result.load();

                if (prev != 0 && prev < *res)
                {
                    return;
                }
            } while (!result.compare_exchange_weak(prev, *res));
        }
    }
}

template <ssize_t N> ssize_t parallelSolve(const std::string& prefix)
{
    constexpr ssize_t BLOCKSIZE = 10000;
    auto concur = std::thread::hardware_concurrency();
    std::vector<std::thread> miners;
    std::atomic<ssize_t> counter;
    std::atomic<ssize_t> result;
    for (auto _ : std::views::iota(0UL, concur))
    {
        miners.emplace_back(mine<N, BLOCKSIZE>, prefix, std::ref(counter), std::ref(result));
    }
    for (auto& t : miners)
    {
        t.join();
    }
    return result.load();
}

} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto prefix = std::string{trim(slurp(input))};
    constexpr ssize_t PART1_ZEROS = 5;
    constexpr ssize_t PART2_ZEROS = 6;
    // this recomputes the first p1 hashes, but p2 is significantly larger so the duplicate p1 is
    // minor
    auto p1 = parallelSolve<PART1_ZEROS>(prefix);
    auto p2 = parallelSolve<PART2_ZEROS>(prefix);
    return {p1, p2};
}
} // namespace aoc
