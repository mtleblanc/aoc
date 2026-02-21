#include <algorithm>
#include <ranges>
#include <span>
#include <vector>
namespace aoc
{
constexpr auto LEN = 256;
inline std::vector<int> hash(std::span<const int> lengths, int rounds = 1)
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

inline std::vector<int> denseHash(std::span<const int> lengths)
{
    static constexpr auto ROUNDS = 64;
    static constexpr auto CHUNK = 16;
    static constexpr auto SUFFIX = std::array<int, 5>{{17, 31, 73, 47, 23}};
    std::vector<int> seed{lengths.begin(), lengths.end()};
    std::ranges::copy(SUFFIX, std::back_inserter(seed));
#if __cpp_lib_ranges_chunk == 202202L
    return hash(seed, ROUNDS) | std::views::chunk(CHUNK) |
           std::views::transform([](auto r)
                                 { return std::ranges::fold_left(r, 0, std::bit_xor<>()); }) |
           std::ranges::to<std::vector>();
#else
    auto h = hash(seed, ROUNDS);
    auto res = std::vector<int>{};
    for (auto i : std::views::iota(0, std::ssize(h) / CHUNK))
    {
        res.push_back(std::ranges::fold_left(
            h | std::views::drop(i * CHUNK) | std::views::take(CHUNK), 0, std::bit_xor<>()));
    }
    return res;
#endif
}
} // namespace aoc
