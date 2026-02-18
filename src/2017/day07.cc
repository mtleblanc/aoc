#include "aoc.hh"
#include <algorithm>
#include <ctre.hpp>
#include <iostream>
#include <map>
#include <ranges>
#include <set>
#include <sstream>

/* https://adventofcode.com/2017/day/7
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 7;
template <> struct SolutionType<YEAR, DAY>
{
    using type = StringSolution;
};
using Solution = Solution_t<YEAR, DAY>;

namespace
{
struct Program
{
    std::string name;
    int weight;
    int totalWeight;
    std::vector<Program*> children;
};

auto readProgram(std::istream& input)
{
    auto lines = readAllLines(input);
    std::map<std::string, Program> programs;
    for (const auto& p : lines)
    {
        static constexpr auto NAME_AND_WEIGHT =
            ctll::fixed_string(R"((\w*) \((\d+)\)(?: -> (.*))?)");
        auto m = ctre::match<NAME_AND_WEIGHT>(p);
        if (!m)
        {
            throw std::invalid_argument("Could not parse input: " + p);
        }
        auto name = m.get<1>().str();
        auto& program = programs[name];
        program.name = name;
        program.weight = m.get<2>().to_number();
        if (auto children = m.get<3>().to_optional_string())
        {
            std::string child;
            std::stringstream ss{*children};
            while (ss >> child)
            {
                if (child.ends_with(','))
                {
                    child.pop_back();
                }
                program.children.push_back(&programs[child]);
            }
        }
    }
    return programs;
}

void topSortHelper(Program& current, Program*& head, std::set<const Program*>& seen)
{

    if (seen.contains(&current))
    {
        return;
    }
    seen.insert(&current);
    for (auto* p : current.children)
    {
        topSortHelper(*p, head, seen);
    }
    head = &current;
}

auto topSort(std::map<std::string, Program>& programs)
{
    auto seen = std::set<const Program*>{};
    Program* head = nullptr;
    for (auto& [n, p] : programs)
    {
        topSortHelper(p, head, seen);
    }
    return head;
}

void populateTotalWeight(Program& program)
{
    program.totalWeight = program.weight;
    for (auto* p : program.children)
    {
        populateTotalWeight(*p);
        program.totalWeight += p->totalWeight;
    }
}

auto findImbalanceHelper(const Program& program, int target) -> int
{
    if (program.children.empty())
    {
        return target;
    }
    auto nextTarget = static_cast<int>((target - program.weight) / std::ssize(program.children));
    auto isCorrectWeight = [nextTarget](auto* p) { return p->totalWeight == nextTarget; };
    if (std::ranges::none_of(program.children, isCorrectWeight))
    {
        return target - program.totalWeight + program.weight;
    }
    auto incorrectProgram = std::ranges::find_if_not(program.children, [nextTarget](auto* p)
                                                     { return p->totalWeight == nextTarget; });
    return findImbalanceHelper(**incorrectProgram, nextTarget);
}

auto findImbalance(const Program& program) -> std::optional<int>
{
    if (program.children.empty())
    {
        return {};
    }
    if (program.children.size() == 1)
    {
        return findImbalance(*program.children.front());
    }
    if (program.children.size() == 2)
    {
        auto left = findImbalance(*program.children.front());
        auto right = findImbalance(*program.children.back());
        return left ? left : right;
    }
    auto weights = program.children | std::views::transform(&Program::totalWeight);
    auto [min, max] = std::ranges::minmax(weights);
    auto odd = std::ranges::count(weights, min) == 1 ? min : max;
    auto incorrectProgram = std::ranges::find(program.children, odd, &Program::totalWeight);
    return findImbalanceHelper(**incorrectProgram, min + max - odd);
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto programs = readProgram(input);
    auto* head = topSort(programs);
    populateTotalWeight(*head);
    return {head->name, std::to_string(*findImbalance(*head))};
}
} // namespace aoc
