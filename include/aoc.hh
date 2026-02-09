#include <ios>
#include <istream>
#include <sstream>
#include <string>
#include <vector>

namespace aoc
{

template <typename T = ssize_t> struct GeneralSolution
{
    T part1;
    T part2;
    GeneralSolution(T part1 = T{}, T part2 = T{}) : part1{part1}, part2{part2} {}
    GeneralSolution& operator+=(const GeneralSolution& other)
    {
        part1 += other.part1;
        part2 += other.part2;
        return *this;
    }
};

using Solution = GeneralSolution<>;
using StringSolution = GeneralSolution<std::string>;

template <size_t Y, size_t D> struct SolutionType
{
    using type = Solution;
};

template <size_t Y, size_t D> using Solution_t = typename SolutionType<Y, D>::type;

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

inline std::string toHex(ssize_t n)
{
    std::ostringstream ss;
    ss << std::hex << n;
    return ss.str();
}

template <size_t Y, size_t D> Solution_t<Y, D> solve(std::istream& input)
{
    (void)input;
    return {};
}

} // namespace aoc