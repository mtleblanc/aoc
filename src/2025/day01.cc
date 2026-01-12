#include <sstream>
#include <cmath>

#include "aoc.hh"

/* https://adventofcode.com/2025/day/1
 *
 * input:
 * L23
 * R15
 * ...
 *
 * problem: given a dial with 100 locations, starting at location 50, perform the turns from the
 * input part1: how many turns end on 0 part2: how many times does the dial move to 0, including
 * during a turn.  
 */

namespace aoc
{
constexpr size_t YEAR = 2025;
constexpr size_t DAY = 1;

namespace
{
inline auto mod(auto m, auto d)
{
    return ((m % d) + d) % d;
}

template <class T, T N> class Dial
{
    T val;
    size_t passZero{0};
    size_t finishZero{0};

  public:
    Dial(T start) : val{start} {}
    [[nodiscard]] auto value() const noexcept
    {
        return val;
    }
    [[nodiscard]] auto landedZero() const noexcept
    {
        return finishZero;
    }
    [[nodiscard]] auto rotations() const noexcept
    {
        return passZero;
    }
    auto& add(T r)
    {
        // count full rotations
        passZero += std::abs(r / N);
        r %= N;

        val = mod(val + r, N);
        if (val == 0)
        {
            finishZero++;
        }
        // The extra % N is for the case where r < 0, val == 0
        // Essentially when turning left, we should treat 0 as N when checking for wraparound
        if (r > val || r < (val - N) % N)
        {
            passZero++;
        }
        return *this;
    }
};

template <class T, T N> std::istream& operator>>(std::istream& is, Dial<T, N>& d)
{
    char c{};
    T val;
    if (is >> c >> val)
    {
        if (c == 'L')
        {
            d.add(-val);
        }
        else if (c == 'R')
        {
            d.add(val);
        }
        else {
            is.setstate(std::ios_base::failbit);
        }
    }
    return is;
}
} // namespace
template <> Solution solve<YEAR, DAY>(const std::vector<std::string>& lines)
{
    constexpr auto N = 100;
    constexpr auto D = 50;
    Dial<int, N> d{D};
    for (const auto& turn : lines)
    {
        std::istringstream iss{turn};
        iss >> d;
    }
    return Solution{d.landedZero(), d.rotations()};
}
} // namespace aoc