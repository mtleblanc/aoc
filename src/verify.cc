#include "solutions.hh"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

using json = nlohmann::ordered_json;

static const char* GREEN = "\033[32m";
static const char* RED = "\033[31m";
static const char* YELLOW = "\033[33m";
static const char* RESET = "\033[0m";

static std::filesystem::path findProjectRoot()
{
    auto path = std::filesystem::current_path();
    while (!path.empty())
    {
        if (std::filesystem::exists(path / "answers.json"))
            return path;
        if (path == path.parent_path())
            break;
        path = path.parent_path();
    }
    return std::filesystem::current_path();
}

static json loadAnswers(const std::filesystem::path& root)
{
    auto path = root / "answers.json";
    if (!std::filesystem::exists(path))
        return json::object();
    std::ifstream ifs{path};
    return json::parse(ifs);
}

static void saveAnswers(const std::filesystem::path& root, const json& answers)
{
    std::ofstream ofs{root / "answers.json"};
    ofs << answers.dump(2) << "\n";
}

struct Stats
{
    int passed = 0;
    int failed = 0;
    int skipped = 0;
};

template <size_t Y, size_t D>
void verifySolution(const std::filesystem::path& root, const json& answers, Stats& stats)
{
    auto yearKey = std::to_string(Y);
    auto dayKey = std::to_string(D);

    if (!answers.contains(yearKey) || !answers[yearKey].contains(dayKey))
    {
        ++stats.skipped;
        return;
    }

    std::ostringstream dayBuf;
    dayBuf << "input" << std::setfill('0') << std::setw(2) << D;
    auto inputPath = root / "inputs" / std::to_string(Y) / dayBuf.str();
    std::ifstream ifs{inputPath};
    if (!ifs)
    {
        std::cout << YELLOW << "SKIP" << RESET << "  " << Y << " Day " << std::setfill('0')
                  << std::setw(2) << D << "  (no input file)\n";
        ++stats.skipped;
        return;
    }

    auto solution = aoc::solve<Y, D>(ifs);

    std::ostringstream p1Buf, p2Buf;
    p1Buf << solution.part1;
    p2Buf << solution.part2;
    auto p1 = p1Buf.str();
    auto p2 = p2Buf.str();

    auto& expected = answers[yearKey][dayKey];
    auto exp1 = expected[0].get<std::string>();
    auto exp2 = expected[1].get<std::string>();

    bool ok = (p1 == exp1 && p2 == exp2);
    if (ok)
    {
        std::cout << GREEN << "PASS" << RESET << "  " << Y << " Day " << std::setfill('0')
                  << std::setw(2) << D << "\n";
        ++stats.passed;
    }
    else
    {
        std::cout << RED << "FAIL" << RESET << "  " << Y << " Day " << std::setfill('0')
                  << std::setw(2) << D;
        if (p1 != exp1)
            std::cout << "  part1: got " << p1 << " expected " << exp1;
        if (p2 != exp2)
            std::cout << "  part2: got " << p2 << " expected " << exp2;
        std::cout << "\n";
        ++stats.failed;
    }
}

template <size_t Y, size_t D>
void recordSolution(const std::filesystem::path& root, json& answers, Stats& stats)
{
    std::ostringstream dayBuf;
    dayBuf << "input" << std::setfill('0') << std::setw(2) << D;
    auto inputPath = root / "inputs" / std::to_string(Y) / dayBuf.str();
    std::ifstream ifs{inputPath};
    if (!ifs)
    {
        ++stats.skipped;
        return;
    }

    auto solution = aoc::solve<Y, D>(ifs);

    std::ostringstream p1Buf, p2Buf;
    p1Buf << solution.part1;
    p2Buf << solution.part2;
    auto p1 = p1Buf.str();
    auto p2 = p2Buf.str();

    // Skip unimplemented stubs (both parts are default values)
    if ((p1 == "0" || p1.empty()) && (p2 == "0" || p2.empty()))
    {
        std::cout << YELLOW << "SKIP" << RESET << "  " << Y << " Day " << std::setfill('0')
                  << std::setw(2) << D << "  (default values)\n";
        ++stats.skipped;
        return;
    }

    auto yearKey = std::to_string(Y);
    auto dayKey = std::to_string(D);
    answers[yearKey][dayKey] = {p1, p2};

    std::cout << GREEN << "REC " << RESET << "  " << Y << " Day " << std::setfill('0')
              << std::setw(2) << D << "  [" << p1 << ", " << p2 << "]\n";
    ++stats.passed;
}

// Template recursion to iterate over all days in a year
template <size_t Y, bool Record> struct YearHandler
{
    template <size_t D>
    void handleAll(const std::filesystem::path& root, json& answers, Stats& stats)
    {
        if constexpr (D != 0)
        {
            handleAll<D - 1>(root, answers, stats);
            if constexpr (Record)
                recordSolution<Y, D>(root, answers, stats);
            else
                verifySolution<Y, D>(root, answers, stats);
        }
    }
};

int main(int argc, char* argv[])
{
    bool record = false;
    int yearFilter = 0;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--record")
            record = true;
        else if (arg == "--year" && i + 1 < argc)
            yearFilter = std::atoi(argv[++i]);
    }

    auto root = findProjectRoot();
    auto answers = loadAnswers(root);
    Stats stats;

    constexpr size_t DAYS = 25;

    if (record)
    {
        // NOLINTBEGIN
        if (!yearFilter || yearFilter == 2015)
            YearHandler<2015, true>{}.handleAll<DAYS>(root, answers, stats);
        if (!yearFilter || yearFilter == 2016)
            YearHandler<2016, true>{}.handleAll<DAYS>(root, answers, stats);
        if (!yearFilter || yearFilter == 2024)
            YearHandler<2024, true>{}.handleAll<DAYS>(root, answers, stats);
        if (!yearFilter || yearFilter == 2025)
            YearHandler<2025, true>{}.handleAll<DAYS>(root, answers, stats);
        // NOLINTEND

        saveAnswers(root, answers);
        std::cout << "\nRecorded " << stats.passed << " solutions to answers.json\n";
    }
    else
    {
        // NOLINTBEGIN
        if (!yearFilter || yearFilter == 2015)
            YearHandler<2015, false>{}.handleAll<DAYS>(root, answers, stats);
        if (!yearFilter || yearFilter == 2016)
            YearHandler<2016, false>{}.handleAll<DAYS>(root, answers, stats);
        if (!yearFilter || yearFilter == 2024)
            YearHandler<2024, false>{}.handleAll<DAYS>(root, answers, stats);
        if (!yearFilter || yearFilter == 2025)
            YearHandler<2025, false>{}.handleAll<DAYS>(root, answers, stats);
        // NOLINTEND

        std::cout << "\n"
                  << stats.passed << " passed, " << stats.failed << " failed, " << stats.skipped
                  << " skipped\n";
    }

    return stats.failed > 0 ? 1 : 0;
}
