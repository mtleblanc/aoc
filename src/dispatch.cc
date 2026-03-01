#include "dispatch.hh"
#include "solutions.hh"
#include <array>
#include <sstream>
#include <utility>

namespace
{

using SolverFn = std::pair<std::string, std::string> (*)(std::istream&);

constexpr size_t NUM_DAYS = 25;

template <size_t Y, size_t D>
std::pair<std::string, std::string> doSolve(std::istream& is)
{
    auto sol = aoc::solve<Y, D>(is);
    std::ostringstream p1;
    std::ostringstream p2;
    p1 << sol.part1;
    p2 << sol.part2;
    return {p1.str(), p2.str()};
}

// Pack expansion over Ds = 0..24 to produce [doSolve<Y,1>, ..., doSolve<Y,25>]
template <size_t Y, size_t... Ds>
std::array<SolverFn, sizeof...(Ds)> makeDayTable(std::index_sequence<Ds...> /*unused*/)
{
    return {doSolve<Y, Ds + 1>...};
}

template <size_t Y>
const std::array<SolverFn, NUM_DAYS>& dayTable()
{
    static const auto table = makeDayTable<Y>(std::make_index_sequence<NUM_DAYS>{});
    return table;
}

} // namespace

namespace aoc
{

std::pair<std::string, std::string> runSolver(size_t year, size_t day, std::istream& input)
{
    if (day < 1 || day > NUM_DAYS)
    {
        return {"", ""};
    }

    // NOLINTBEGIN
    switch (year)
    {
    case 2015: return dayTable<2015>()[day - 1](input);
    case 2016: return dayTable<2016>()[day - 1](input);
    case 2017: return dayTable<2017>()[day - 1](input);
    case 2018: return dayTable<2018>()[day - 1](input);
    case 2024: return dayTable<2024>()[day - 1](input);
    case 2025: return dayTable<2025>()[day - 1](input);
    // NOLINTEND
    default: return {"", ""};
    }
}

} // namespace aoc
