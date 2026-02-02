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

    for (std::string tmp; std::getline(input, tmp);)
    {
        lines.push_back(std::move(tmp));
    }
    return lines;
}

template <typename T> inline std::vector<T> readAll(std::istream& input)
{
    std::vector<T> res;
    for (T t; input >> t;)
    {
        res.push_back(std::move(t));
    }
    return res;
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

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers)
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

template <> Solution solve<2016, 1>(std::istream& input);
template <> Solution solve<2016, 2>(std::istream& input);
template <> Solution solve<2016, 3>(std::istream& input);
template <> Solution solve<2016, 4>(std::istream& input);
template <> Solution solve<2016, 5>(std::istream& input);
template <> Solution solve<2016, 6>(std::istream& input);
template <> Solution solve<2016, 7>(std::istream& input);
template <> Solution solve<2016, 8>(std::istream& input);
template <> Solution solve<2016, 9>(std::istream& input);
template <> Solution solve<2016, 10>(std::istream& input);
template <> Solution solve<2016, 11>(std::istream& input);
template <> Solution solve<2016, 12>(std::istream& input);
template <> Solution solve<2016, 13>(std::istream& input);
template <> Solution solve<2016, 14>(std::istream& input);
template <> Solution solve<2016, 15>(std::istream& input);
template <> Solution solve<2016, 16>(std::istream& input);
template <> Solution solve<2016, 17>(std::istream& input);
template <> Solution solve<2016, 18>(std::istream& input);
template <> Solution solve<2016, 19>(std::istream& input);
template <> Solution solve<2016, 20>(std::istream& input);
template <> Solution solve<2016, 21>(std::istream& input);
template <> Solution solve<2016, 22>(std::istream& input);
template <> Solution solve<2016, 23>(std::istream& input);
template <> Solution solve<2016, 24>(std::istream& input);
template <> Solution solve<2016, 25>(std::istream& input);

template <> Solution solve<2024, 1>(std::istream& input);
template <> Solution solve<2024, 2>(std::istream& input);
template <> Solution solve<2024, 3>(std::istream& input);
template <> Solution solve<2024, 4>(std::istream& input);
template <> Solution solve<2024, 5>(std::istream& input);
template <> Solution solve<2024, 6>(std::istream& input);
template <> Solution solve<2024, 7>(std::istream& input);
template <> Solution solve<2024, 8>(std::istream& input);
template <> Solution solve<2024, 9>(std::istream& input);
template <> Solution solve<2024, 10>(std::istream& input);
template <> Solution solve<2024, 11>(std::istream& input);
template <> Solution solve<2024, 12>(std::istream& input);
template <> Solution solve<2024, 13>(std::istream& input);
template <> Solution solve<2024, 14>(std::istream& input);
template <> Solution solve<2024, 15>(std::istream& input);
template <> Solution solve<2024, 16>(std::istream& input);
template <> Solution solve<2024, 17>(std::istream& input);
template <> Solution solve<2024, 18>(std::istream& input);
template <> Solution solve<2024, 19>(std::istream& input);
template <> Solution solve<2024, 20>(std::istream& input);
template <> Solution solve<2024, 21>(std::istream& input);
template <> Solution solve<2024, 22>(std::istream& input);
template <> Solution solve<2024, 23>(std::istream& input);
template <> Solution solve<2024, 24>(std::istream& input);
template <> Solution solve<2024, 25>(std::istream& input);

template <> Solution solve<2025, 1>(std::istream& input);
template <> Solution solve<2025, 2>(std::istream& input);
template <> Solution solve<2025, 3>(std::istream& input);
template <> Solution solve<2025, 4>(std::istream& input);
template <> Solution solve<2025, 5>(std::istream& input);
template <> Solution solve<2025, 6>(std::istream& input);
template <> Solution solve<2025, 7>(std::istream& input);
template <> Solution solve<2025, 8>(std::istream& input);
template <> Solution solve<2025, 9>(std::istream& input);
template <> Solution solve<2025, 10>(std::istream& input);
template <> Solution solve<2025, 11>(std::istream& input);
template <> Solution solve<2025, 12>(std::istream& input);
// NOLINTEND

} // namespace aoc