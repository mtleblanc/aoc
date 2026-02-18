#include "aoc.hh"
#include <algorithm>
#include <cassert>
#include <ctre.hpp>
#include <iostream>
#include <map>
#include <optional>
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
    std::optional<int> totalWeight;
    std::vector<Program*> children;

    [[nodiscard]] auto childWeights() const
    {
        return children | std::views::transform([](auto c) { return *c->totalWeight; });
    }
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

void visit(Program& current)
{
    current.totalWeight = current.weight;
    for (auto* p : current.children)
    {
        if (!p->totalWeight)
        {
            visit(*p);
        }
        *current.totalWeight += *p->totalWeight;
    }
}

// assumes single rooted tree, does not detect cycles or multiple roots
// uses std::optional totalWeight to record visting nodes
auto findRootAndPopulateWeights(std::map<std::string, Program>& programs)
{
    Program* head = nullptr;
    for (auto& [n, p] : programs)
    {
        if (!p.totalWeight)
        {
            head = &p;
            visit(p);
        }
    }
    return head;
}

// auto oddOneOut(const std::ranges::forward_range auto& r, auto proj)
// {
//     auto it = r.begin();
//     if (it == r.end())
//     {
//         return r;
//     }
//     auto first = std::invoke(proj, *it);
//     if (++it == r.end())
//     {
//         return r.begin();
//     }
//     auto second = std::invoke(proj, *it);
//     if (first == second)
//     {
//         return (std::ranges::find_if_not(
//             it, r.end(), [&first](const auto& e) { return e != first; }, proj));
//     }
//     if (++it == r.end())
//     {
//         // first or second both valid returns
//         return r.begin();
//     }
//     return first == std::invoke(proj, *it) ? ++r.begin() : r.begin();
// }

auto allEqual(const std::ranges::input_range auto& r, auto proj)
{
    auto it = r.begin();
    if (it == r.end())
    {
        return true;
    }
    const auto& val = std::invoke(proj, *it);
    return std::ranges::all_of(++it, r.end(), [&val](auto v) { return v == val; }, proj);
}

auto findImbalanceHelper(const Program& program, int target) -> int
{
    if (allEqual(program.children, &Program::totalWeight))
    {
        return target - std::ranges::fold_left(program.childWeights(), 0, std::plus<>());
    }
    auto nextTarget = static_cast<int>((target - program.weight) / std::ssize(program.children));
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
        // this is incorrect if the imbalanced weight has no disks below it holding 3 or more
        // programs
        auto left = findImbalance(*program.children.front());
        auto right = findImbalance(*program.children.back());
        return left ? left : right;
    }
    auto weights = program.children | std::views::transform([](auto p) { return *p->totalWeight; });
    auto [min, max] = std::ranges::minmax(weights);
    auto odd = std::ranges::count(weights, min) == 1 ? min : max;
    auto incorrectProgram = std::ranges::find(program.children, odd, &Program::totalWeight);
    return findImbalanceHelper(**incorrectProgram, min + max - odd);
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto programs = readProgram(input);
    auto* root = findRootAndPopulateWeights(programs);
    assert(root != nullptr);
    return {root->name, std::to_string(*findImbalance(*root))};
}
} // namespace aoc
