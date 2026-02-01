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

inline std::string_view trim(std::string_view s)
{
    auto start = s.find_first_not_of(" \t\n\r");
    if (start == std::string_view::npos)
    {
        return {};
    }
    auto end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

template <size_t Y, size_t D> Solution solve(std::istream& input)
{
    (void)input;
    return {};
}

// NOLINTBEGIN(readability-magic-numbers)
// Forward declarations for 2015
template <> Solution solve<2015, 1>(std::istream& input);
template <> Solution solve<2015, 2>(std::istream& input);
template <> Solution solve<2015, 3>(std::istream& input);
template <> Solution solve<2015, 4>(std::istream& input);
template <> Solution solve<2015, 5>(std::istream& input);
template <> Solution solve<2015, 6>(std::istream& input);
template <> Solution solve<2015, 7>(std::istream& input);
template <> Solution solve<2015, 8>(std::istream& input);
template <> Solution solve<2015, 9>(std::istream& input);
template <> Solution solve<2015, 10>(std::istream& input);
template <> Solution solve<2015, 11>(std::istream& input);
template <> Solution solve<2015, 12>(std::istream& input);
template <> Solution solve<2015, 13>(std::istream& input);
template <> Solution solve<2015, 14>(std::istream& input);
template <> Solution solve<2015, 15>(std::istream& input);
template <> Solution solve<2015, 16>(std::istream& input);
template <> Solution solve<2015, 17>(std::istream& input);
template <> Solution solve<2015, 18>(std::istream& input);
template <> Solution solve<2015, 19>(std::istream& input);
template <> Solution solve<2015, 20>(std::istream& input);
template <> Solution solve<2015, 21>(std::istream& input);
template <> Solution solve<2015, 22>(std::istream& input);
template <> Solution solve<2015, 23>(std::istream& input);
template <> Solution solve<2015, 24>(std::istream& input);
template <> Solution solve<2015, 25>(std::istream& input);
// NOLINTEND(readability-magic-numbers)

} // namespace aoc