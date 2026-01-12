#include <cassert>
#include <iostream>
#include <sstream>

#include "aoc.hh"

namespace aoc
{
constexpr size_t YEAR = 2025;
constexpr size_t DAY = 1;
template <typename T, T N> class Dial
{
    T val;
    size_t passZero{0};
    size_t finishZero{0};

  public:
    Dial(T start) : val{start} {}
    T value()
    {
        return val;
    }
    size_t landedZero()
    {
        return finishZero;
    }
    size_t rotations()
    {
        return passZero;
    }
    Dial& add(T r)
    {
        // one rotation at a time to track down what ended up being an off-by-one
#ifndef NDEBUG
        T tpasses{0};
        if (r < 0)
        {
            T tval{val == 0 ? N : val};
            T tr{-r};
            while (tr > N)
            {
                tr -= N;
                tpasses++;
            }
            if (tval <= tr)
            {
                tpasses++;
            }
        }
        else if (r > 0)
        {
            T tval{val};
            T tr{r};
            while (tr > N)
            {
                tr -= N;
                tpasses++;
            }
            if (tval + tr >= N)
            {
                tpasses++;
            }
        }
#endif
        T oval{val};
        val += r;
        T nval{val};
        // annoying truncate-towards-zero
        val %= N;
        val += N;
        val %= N;
        if (val == 0)
        {
            finishZero++;
        }
        assert(val < N && 0 <= val);
        // adjust values to be non-negative so that /N works as floor, and increasing so that
        // starting/ending at 0 is counted properly
        if (nval < oval)
        {
            nval = -nval;
            oval = -oval;
        }
        if (oval < 0)
        {
            T mult = nval / N + 1;
            nval += mult * N;
            oval += mult * N;
        }
        assert(nval > oval);
        assert(oval >= 0);
        assert(abs(nval - oval) == abs(r));
        T passes = (nval / N) - ((oval) / N);
        passZero += passes;
        assert(passes == tpasses);
#ifdef DEBUG
        std::cout << " landed on " << val;
        if (passes)
        {
            std::cout << " passed zero " << passes << " times";
        }
        std::cout << std::endl;
#endif
        return *this;
    }
};

template <typename T, T N> std::istream& operator>>(std::istream& is, Dial<T, N>& d)
{
    char c{};
    T val;
    is >> c >> val;
    switch (c)
    {
    case 'L':
    case 'l':
#ifdef DEBUG
        std::cout << "Moving L" << val;
#endif
        d.add(-val);
        break;
    case 'R':
    case 'r':
#ifdef DEBUG
        std::cout << "Moving R" << val;
#endif
        d.add(val);
        break;
    case '\0':
        break;
    default:
        throw 0;
    }
    return is;
}
template <> Solution solve<YEAR, DAY>(const std::vector<std::string>& lines)
{
    constexpr auto N = 100;
    constexpr auto D = 50;
    using std::endl;
    Dial<int, N> d{D};
    for (const auto& turn : lines)
    {
        std::istringstream iss{turn};
        iss >> d;
    }
    return Solution{d.landedZero(), d.rotations()};
}
} // namespace aoc