#include "aoc.hh"
#include <fstream>
#include <iostream>
#include <sstream>

template <size_t Y, size_t D> void printSolution(bool useSample)
{
    std::ostringstream oss{};
    oss << "inputs/" << Y << "/" << (useSample ? "sample" : "input") << std::setfill('0')
        << std::setw(2) << D << ".txt";
    std::ifstream ifs{oss.str()};
    std::vector<std::string> lines;
    std::string tmp;
    while (getline(ifs, tmp))
    {
        lines.push_back(std::move(tmp));
    }
    auto solution = aoc::solve<Y, D>(lines);
    std::cout << Y << " Day " << D << " part 1: " << solution.part1 << "\n";
    std::cout << Y << " Day " << D << " part 2: " << solution.part2 << "\n";
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
    constexpr auto YEAR = 2025;
    constexpr auto DAYS = 12;
    YearPrinter<YEAR>{}.printAll<DAYS>(false);
}