#include "solutions.hh"
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>

template <size_t Y, size_t D> void printSolution(bool useSample)
{
    std::ostringstream oss{};
    oss << "inputs/" << Y << "/" << (useSample ? "sample" : "input") << std::setfill('0')
        << std::setw(2) << D;
    std::ifstream ifs{oss.str()};

    auto start = std::chrono::high_resolution_clock::now();
    auto solution = aoc::solve<Y, D>(ifs);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // NOLINTBEGIN
    std::cout << Y << " Day " << std::setfill('0') << std::setw(2) << D
              << " part 1: " << std::setfill(' ') << std::setw(10) << solution.part1 << "\t"
              << " part 2: " << std::setfill(' ') << std::setw(10) << solution.part2 << "\t"
              << " (" << duration.count() << " μs)\n";
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
    constexpr auto YEAR = 2016;
    constexpr auto DAYS = 25;
    YearPrinter<YEAR>{}.printAll<DAYS>(false);
}