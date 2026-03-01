#include "dispatch.hh"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

namespace
{
using json = nlohmann::ordered_json;

constexpr auto GREEN = "\033[32m";
constexpr auto RED = "\033[31m";
constexpr auto YELLOW = "\033[33m";
constexpr auto RESET = "\033[0m";

std::filesystem::path findProjectRoot()
{
    auto path = std::filesystem::current_path();
    while (!path.empty())
    {
        if (std::filesystem::exists(path / "answers.json"))
        {
            return path;
        }
        if (path == path.parent_path())
        {
            break;
        }
        path = path.parent_path();
    }
    return std::filesystem::current_path();
}

json loadAnswers(const std::filesystem::path& root)
{
    auto path = root / "answers.json";
    if (!std::filesystem::exists(path))
    {
        return json::object();
    }
    std::ifstream ifs{path};
    return json::parse(ifs);
}

void saveAnswers(const std::filesystem::path& root, const json& answers)
{
    std::ofstream ofs{root / "answers.json"};
    ofs << answers.dump(2) << "\n";
}

std::filesystem::path inputPath(const std::filesystem::path& root, size_t year, size_t day)
{
    std::ostringstream name;
    name << "input" << std::setfill('0') << std::setw(2) << day;
    return root / "inputs" / std::to_string(year) / name.str();
}

struct Stats
{
    int passed = 0;
    int failed = 0;
    int skipped = 0;
};

void processDay(bool record, size_t year, size_t day, const std::filesystem::path& root,
                json& answers, Stats& stats)
{
    auto yearKey = std::to_string(year);
    auto dayKey = std::to_string(day);

    if (!record && (!answers.contains(yearKey) || !answers[yearKey].contains(dayKey)))
    {
        ++stats.skipped;
        return;
    }

    std::ifstream ifs{inputPath(root, year, day)};
    if (!ifs)
    {
        if (!record)
        {
            std::cout << YELLOW << "SKIP" << RESET << "  " << year << " Day " << std::setfill('0')
                      << std::setw(2) << day << "  (no input file)\n";
        }
        ++stats.skipped;
        return;
    }

    auto [p1, p2] = aoc::runSolver(year, day, ifs);

    if (record)
    {
        // Skip unimplemented stubs (both parts are default values)
        if ((p1 == "0" || p1.empty()) && (p2 == "0" || p2.empty()))
        {
            std::cout << YELLOW << "SKIP" << RESET << "  " << year << " Day " << std::setfill('0')
                      << std::setw(2) << day << "  (default values)\n";
            ++stats.skipped;
            return;
        }
        answers[yearKey][dayKey] = {p1, p2};
        std::cout << GREEN << "REC " << RESET << "  " << year << " Day " << std::setfill('0')
                  << std::setw(2) << day << "  [" << p1 << ", " << p2 << "]\n";
        ++stats.passed;
    }
    else
    {
        const auto& expected = answers[yearKey][dayKey];
        auto exp1 = expected[0].get<std::string>();
        auto exp2 = expected[1].get<std::string>();
        bool ok = (p1 == exp1 && p2 == exp2);
        if (ok)
        {
            std::cout << GREEN << "PASS" << RESET << "  " << year << " Day " << std::setfill('0')
                      << std::setw(2) << day << "\n";
            ++stats.passed;
        }
        else
        {
            std::cout << RED << "FAIL" << RESET << "  " << year << " Day " << std::setfill('0')
                      << std::setw(2) << day;
            if (p1 != exp1)
            {
                std::cout << "  part1: got " << p1 << " expected " << exp1;
            }
            if (p2 != exp2)
            {
                std::cout << "  part2: got " << p2 << " expected " << exp2;
            }
            std::cout << "\n";
            ++stats.failed;
        }
    }
}
} // namespace

int main(int argc, char* argv[])
{
    bool record = false;
    size_t yearFilter = 0;

    // NOLINTBEGIN(*-pointer-arithmetic)
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--record")
        {
            record = true;
        }
        else if (arg == "--year" && i + 1 < argc)
        {
            yearFilter = static_cast<size_t>(std::atoi(argv[++i]));
        }
    }
    // NOLINTEND(*-pointer-arithmetic)

    auto root = findProjectRoot();
    auto answers = loadAnswers(root);
    Stats stats;

    constexpr size_t DAYS = 25;

    auto runYear = [&](size_t year, auto fn)
    {
        if (yearFilter != 0 && yearFilter != year)
        {
            return;
        }
        for (size_t day = 1; day <= DAYS; ++day)
        {
            fn(year, day);
        }
    };

    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
    for (size_t year : {2015UL, 2016UL, 2017UL, 2018UL, 2024UL, 2025UL})
    {
        runYear(year, [&](size_t y, size_t d) { processDay(record, y, d, root, answers, stats); });
    }
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

    if (record)
    {
        saveAnswers(root, answers);
        std::cout << "\nRecorded " << stats.passed << " solutions to answers.json\n";
    }
    else
    {
        std::cout << "\n"
                  << stats.passed << " passed, " << stats.failed << " failed, " << stats.skipped
                  << " skipped\n";
    }

    return stats.failed > 0 ? 1 : 0;
}
