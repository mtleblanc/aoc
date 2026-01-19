#include "aoc.hh"
#include <fstream>
#include <iostream>
#include <sstream>

template <size_t Y, size_t D> void printSolution(bool useSample)
{
    std::ostringstream oss{};
    oss << "inputs/" << Y << "/" << (useSample ? "sample" : "input") << std::setfill('0')
        << std::setw(2) << D;
    std::ifstream ifs{oss.str()};
    auto solution = aoc::solve<Y, D>(ifs);

    // NOLINTBEGIN
    std::cout << Y << " Day " << std::setfill('0') << std::setw(2) << D
              << " part 1: " << std::setfill(' ') << std::setw(10) << solution.part1 << "\t"
              << " part 2: " << solution.part2 << "\n";
    // NOLINTEND
}

template <size_t Y> struct YearPrinter
{
    template <size_t D> void printAll(bool useSample)
    {
        printAll<D - 1>(useSample);
        printSolution<Y, D>(useSample);
    }

    template <> void printAll<0>(bool useSample)
    {
        (void)useSample;
    }
};

int main()
{
    constexpr auto YEAR = 2015;
    constexpr auto DAYS = 25;
    YearPrinter<YEAR>{}.printAll<DAYS>(false);
}