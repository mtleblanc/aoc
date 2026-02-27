#include "aoc.hh"
#include "knothash.hh"
#include "util.hh"
#include <cassert>
#include <map>
#include <set>

/* https://adventofcode.com/2017/day/14
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 14;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
auto generateGrid(std::string_view seed)
{
    std::vector<std::vector<int>> hashes;
    static constexpr auto ROWS = 128;
    std::string base{seed};
    base += "-";
    for (auto i : std::views::iota(0, ROWS))
    {
        auto rowSeed = base + std::to_string(i);
        auto lengths = std::ranges::to<std::vector>(
            rowSeed | std::views::transform([](auto c) -> int { return c; }));
        auto hash = denseHash(lengths);
        hashes.emplace_back(denseHash(lengths));
    }
    return hashes;
}

auto part1(const auto& grid)
{
    auto bits = int{};
    for (const auto& hash : grid)
    {
        auto setBits = hash | std::views::transform(
                                  [](auto h) { return std::popcount(static_cast<unsigned>(h)); });
        bits += std::ranges::fold_left(setBits, 0, std::plus<>());
    }
    return bits;
}

class Node
{
    int groupSize_{1};
    Node* rep_;

  public:
    Node() : rep_{this} {}

    Node* representative()
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

    void merge(Node& o)
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

auto part2(auto grid)
{
    static constexpr auto BITS = 8;
    auto stringGrid = std::ranges::to<std::vector>(
        grid | std::views::transform(
                   [](const auto& row)
                   {
                       return std::ranges::to<std::string>(std::views::join(
                           std::views::transform(row, [](auto n) { return toBin<BITS>(n); })));
                   }));
    std::map<std::pair<int, int>, Node> nodes;
    for (const auto& [y, row] : std::views::zip(std::views::iota(0), stringGrid))
    {
        for (const auto& [x, n] : std::views::zip(std::views::iota(0), row))
        {
            if (n != '1')
            {
                continue;
            }
            auto& node = nodes[{x, y}];
            if (x > 0 && stringGrid[y][x - 1] == '1')
            {
                node.merge(nodes[{x - 1, y}]);
            }
            if (x + 1 < std::ssize(stringGrid) && stringGrid[y][x + 1] == '1')
            {
                node.merge(nodes[{x + 1, y}]);
            }
            if (y > 0 && stringGrid[y - 1][x] == '1')
            {
                node.merge(nodes[{x, y - 1}]);
            }
            if (y + 1 < std::ssize(row) && stringGrid[y + 1][x] == '1')
            {
                node.merge(nodes[{x, y + 1}]);
            }
        }
    }
    auto groups = std::ranges::to<std::set>(nodes | std::views::values |
                                            std::views::transform(&Node::representative));
    return std::ssize(groups);
}
} // namespace
template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto text = trim(slurp(input));
    auto grid = generateGrid(text);

    return {part1(grid), static_cast<int>(part2(grid))};
}
} // namespace aoc
