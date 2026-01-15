#include <ios>
#include <istream>
#include <string>
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

inline std::vector<std::string> readAllLines(std::istream& input)
{
    std::vector<std::string> lines;
    std::string tmp;
    while (std::getline(input, tmp))
    {
        lines.push_back(std::move(tmp));
    }
    return lines;
}

inline std::string slurp(std::istream& input)
{
    auto cur = input.tellg();
    input.seekg(0, std::ios_base::seekdir::end);
    auto end = input.tellg();
    input.seekg(cur, std::ios_base::seekdir::beg);
    std::string ret("\0", end - cur);
    input.read(ret.data(), end - cur);
    return ret;
}

template <size_t Y, size_t D> Solution solve(std::istream& input)
{
    (void)input;
    return {};
}
} // namespace aoc