#include "aoc.hh"
#include <algorithm>
#include <cassert>
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
class ProgramTower
{
  public:
    struct Program;

  private:
    std::map<std::string, Program> programs_;
    Program* root_{};

    void loadSkeleton(std::span<std::string> description)
    {
        for (const auto& p : description)
        {
            static constexpr auto NAME_AND_WEIGHT =
                ctll::fixed_string(R"((\w*) \((\d+)\)(?: -> (.*))?)");
            auto m = ctre::match<NAME_AND_WEIGHT>(p);
            if (!m)
            {
                throw std::invalid_argument("Could not parse input: " + p);
            }
            auto name = m.get<1>().str();
            auto& program = programs_[name];
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
                    program.children.push_back(&programs_[child]);
                }
            }
        }
    }

    void visit(Program& current, std::set<Program*>& visited)
    {
        current.totalWeight = current.weight;
        for (auto* p : current.children)
        {
            if (!visited.contains(p))
            {
                visit(*p, visited);
            }
            current.totalWeight += p->totalWeight;
        }
        visited.insert(&current);
    }

    void construct(std::ranges::input_range auto&& programs)
    {
        std::set<Program*> visited;
        for (auto* p : programs)
        {
            if (!visited.contains(p))
            {
                root_ = p;
                visit(*p, visited);
            }
        }
    }

  public:
    ProgramTower(std::span<std::string> description)
    {
        loadSkeleton(description);
        construct(programs_ | std::views::transform([](auto& p) { return &p.second; }));
    }

    [[nodiscard]] const Program* root() const
    {
        return root_;
    }

    [[nodiscard]] Program* root()
    {
        return root_;
    }

    struct Program
    {
        std::string name;
        int weight{};
        int totalWeight{};
        std::vector<Program*> children;

        [[nodiscard]] auto childWeights() const
        {
            return children | std::views::transform([](auto c) { return c->totalWeight; });
        }
    };
};

// Included for completeness, only needed if the imbalanced node is directly connected to the root,
// which is not the case for the input
template <typename P = std::identity>
auto oddOneOut(const std::ranges::forward_range auto& r, P proj)
{
    auto it = r.begin();
    if (it == r.end())
    {
        return r.end();
    }
    auto first = std::invoke(proj, *it);
    if (++it == r.end())
    {
        return r.begin();
    }
    auto second = std::invoke(proj, *it);
    if (first == second)
    {
        return (std::ranges::find_if_not(
            it, r.end(), [&first](const auto& e) { return e == first; }, proj));
    }
    if (++it == r.end())
    {
        // first or second both valid returns
        return r.begin();
    }
    return first == std::invoke(proj, *it) ? ++r.begin() : r.begin();
}

template <typename P = std::identity>
auto allEqual(const std::ranges::input_range auto& r, P proj = {})
{
    auto it = r.begin();
    if (it == r.end())
    {
        return true;
    }
    const auto val = std::invoke(proj, *it);
    return std::ranges::all_of(++it, r.end(), [&val](const auto& v) { return v == val; }, proj);
}

constexpr auto BALANCED = [](auto* p) { return allEqual(p->childWeights()); };
constexpr auto TOTAL_WEIGHT = &ProgramTower::Program::totalWeight;

auto correctImbalanceAtRoot(const ProgramTower::Program* program)
{
    const auto& kids = program->children;
    assert(kids.size() > 1);
    auto bad = oddOneOut(kids, TOTAL_WEIGHT);
    if (bad == kids.end())
    {
        throw std::invalid_argument("No imbalance at root");
    }
    auto other = kids.begin();
    if (other == bad)
    {
        ++other;
    }
    return (*bad)->weight + (*other)->totalWeight - (*bad)->totalWeight;
}

auto correctImbalance(const ProgramTower::Program* program, int target)
{
    for (;;)
    {
        const auto& kids = program->children;
        if (allEqual(kids, TOTAL_WEIGHT))
        {
            return target - program->totalWeight + program->weight;
        }
        target -= program->weight;
        target /= static_cast<int>(std::ssize(kids));
        auto next = std::ranges::find_if_not(
            kids, [target](const auto& v) { return v == target; }, TOTAL_WEIGHT);
        assert(next != program->children.end());
        program = *next;
    }
}

auto correctImbalance(const ProgramTower::Program* program)
{
    while (program->children.size() == 1)
    {
        program = program->children.front();
    }
    assert(!BALANCED(program));
    const auto& kids = program->children;
    auto next = std::ranges::find_if_not(kids, BALANCED);
    if (next != kids.end())
    {
        auto other = kids.begin();
        // kids contains next, so not empty
        assert(other != kids.end());
        if (other == next)
        {
            ++other;
        }
        // kids does not have size 1 from while loop
        assert(other != kids.end());
        auto target = (*other)->totalWeight;
        return correctImbalance(*next, target);
    }
    return correctImbalanceAtRoot(program);
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto lines = readAllLines(input);
    auto programTower = ProgramTower{lines};
    assert(programTower.root() != nullptr);
    return {programTower.root()->name, std::to_string(correctImbalance(programTower.root()))};
}
} // namespace aoc
