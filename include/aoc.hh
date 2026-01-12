#include <vector>

namespace aoc
{
struct Solution
{
    size_t part1;
    size_t part2;
    Solution(size_t part1 = 0, size_t part2 = 0) : part1{part1}, part2{part2} {}
    Solution& operator+=(const Solution& other)
    {
        part1 += other.part1;
        part2 += other.part2;
        return *this;
    }
};

template <size_t Y, size_t D> Solution solve(const std::vector<std::string>& lines)
{
    (void)lines;
    return {};
}
} // namespace aoc