#include <algorithm>
#include <ranges>
#include <span>
#include <vector>
namespace aoc
{
constexpr auto LEN = 256;
inline std::vector<int> hash(std::span<int> lengths, int rounds = 1)
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

inline std::vector<int> denseHash(std::span<int> lengths)
{
    static constexpr auto ROUNDS = 64;
    static constexpr auto CHUNK = 16;
    return hash(lengths, ROUNDS) | std::views::chunk(CHUNK) |
           std::views::transform([](auto r)
                                 { return std::ranges::fold_left(r, 0, std::bit_xor<>()); }) |
           std::ranges::to<std::vector>();
}
} // namespace aoc
