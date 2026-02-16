#include "solutions.hh"
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

namespace
{
template <size_t Y, size_t D> void printSolution(bool useSample, bool includeSlow)
{
    if constexpr (aoc::IsSlow<Y, D>::value)
    {
        if (!includeSlow)
        {
            // NOLINTBEGIN
            std::cout << Y << " Day " << std::setfill('0') << std::setw(2) << D
                      << " part 1: " << std::setfill(' ') << std::setw(18) << "SKIP" << "\t"
                      << " part 2: " << std::setfill(' ') << std::setw(18) << "SKIP" << "\t"
                      << " (slow)\n";
            // NOLINTEND
            return;
        }
    }

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
              << " part 1: " << std::setfill(' ') << std::setw(18) << solution.part1 << "\t"
              << " part 2: " << std::setfill(' ') << std::setw(18) << solution.part2 << "\t" << " ("
              << duration.count() << " μs)\n";
    // NOLINTEND
}

template <size_t Y> struct YearPrinter
{
    template <size_t D> void printAll(bool useSample, bool includeSlow)
    {
        if constexpr (D > 0)
        {
            printAll<D - 1>(useSample, includeSlow);
            printSolution<Y, D>(useSample, includeSlow);
        }
    }
};
} // namespace

int main(int argc, char* argv[])
{
    constexpr auto YEAR = 2016;
    constexpr auto DAYS = 25;

    bool includeSlow = false;
    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]) ==
            "--slow") // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        {
            includeSlow = true;
        }
    }

    YearPrinter<YEAR>{}.printAll<DAYS>(false, includeSlow);
}
