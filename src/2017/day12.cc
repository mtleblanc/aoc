#include "aoc.hh"
#include <cassert>
#include <ctre.hpp>
#include <map>
#include <ranges>
#include <set>

/* https://adventofcode.com/2017/day/12
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 12;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
class Program
{
    int groupSize_{1};
    Program* rep_;

  public:
    Program() : rep_{this} {}

    Program* representative()
    {
        auto* p = rep_;
        while (p->rep_ != p)
        {
            p->rep_ = p->rep_->rep_;
            p = p->rep_;
        }
        return rep_ = p->rep_;
    }

    [[nodiscard]] int size()
    {
        return representative()->groupSize_;
    }

    void merge(Program& o)
    {
        auto* left = o.representative();
        auto* right = representative();
        assert(left->rep_ == left && right->rep_ == right);
        if (left == right)
        {
            return;
        }
        if (right->groupSize_ > left->groupSize_)
        {
            std::swap(left, right);
        }
        left->groupSize_ += right->groupSize_;
        right->rep_ = left;
    }
};

class Network
{
    std::map<int, Program> programs;

    static constexpr auto PAT = ctll::fixed_string(R"((\d+))");

  public:
    Network() = default;

    Program& operator[](int id)
    {
        return programs[id];
    }

    void parseConnection(std::string_view text)
    {
        auto ids = ctre::search_all<PAT>(text);
        if (ids.begin() == ids.end())
        {
            return;
        }
        auto it = ids.begin();
        auto lu = [this](const auto& it) -> auto&
        { return this->programs[(*it).template get<1>().to_number()]; };
        auto& p = lu(it);
        while (++it != ids.end())
        {
            auto& conn = lu(it);
            p.merge(conn);
        }
    }

    std::set<Program*> groups()
    {
        // a bit of fragility here.  &Program::representative mutates both the item being iterated
        // over and others, while these items are being put into a set.
        // correctness depends on the pointers themselves not being invalidated or modified
        return std::ranges::to<std::set>(programs | std::views::values |
                                         std::views::transform(&Program::representative));
    }
};
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto lines = readAllLines(input);
    Network n;

    for (const auto& l : lines)
    {
        n.parseConnection(l);
    }

    return {n[0].size(), static_cast<int>(n.groups().size())};
}
} // namespace aoc
