#include <cassert>
#include <iostream>
#include <machine/limits.h>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

#include "aoc.hh"
namespace aoc
{
constexpr size_t YEAR = 2025;
constexpr size_t DAY = 12;
namespace
{
struct Problem
{
    size_t l{};
    size_t w{};
    std::vector<size_t> presents;
};

std::istream& operator>>(std::istream& is, Problem& n)
{
    is >> n.l;
    char c{};
    is >> c;
    is >> n.w;
    is >> c;
    size_t s{};
    while (is >> s)
    {
        n.presents.push_back(s);
    }
    return is;
}

size_t part1(std::vector<Problem>& ns)
{
    constexpr std::array<size_t, 6> SIZES = {7, 7, 7, 7, 6, 5};
    std::vector<size_t> fits;
    std::ranges::transform(ns, back_inserter(fits),
                           [&SIZES](auto& p)
                           {
                               return std::inner_product(SIZES.begin(), SIZES.end(),
                                                         p.presents.begin(), size_t{}) < p.l * p.w
                                          ? 1
                                          : 0;
                           });
    return std::accumulate(fits.begin(), fits.end(), size_t{});
}
} // namespace
template <> Solution solve<YEAR, DAY>(const std::vector<std::string>& lines)
{
    std::vector<Problem> v;
    for (const auto& s : lines | std::views::drop(30))
    {
        if (s.length() == 0)
        {
            break;
        }
        std::istringstream iss{s};
        v.emplace_back();
        try
        {
            iss >> v.back();
        }
        catch (const std::exception& ex)
        {
            std::cerr << ex.what() << std::endl;
            exit(1);
        }
        catch (...)
        {
            std::cerr << "Unknown exception" << std::endl;
        }
    }

    return Solution { part1(v)};
}
} // namespace aoc