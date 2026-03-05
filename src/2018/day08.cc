#include "aoc.hh"
#include <algorithm>
#include <memory>
#include <ranges>
#include <stdexcept>

/* https://adventofcode.com/2018/day/8
 */
namespace aoc
{
constexpr size_t YEAR = 2018;
constexpr size_t DAY = 8;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
struct Node
{
    std::vector<int> metadata;
    std::vector<std::unique_ptr<Node>> children;
};

std::unique_ptr<Node> parseNode(auto& input)
{
    auto subNodes = 0;
    auto metadata = 0;
    if (!(input >> subNodes >> metadata))
    {
        throw std::invalid_argument{"Unexpected EOF"};
    }
    auto res = std::make_unique<Node>();
    res->children.reserve(subNodes);
    for (auto i = 0; i < subNodes; ++i)
    {
        res->children.push_back(parseNode(input));
    }
    res->metadata.assign(metadata, 0);
    for (auto i = 0; i < metadata; ++i)
    {
        if (!(input >> res->metadata[i]))
        {
            throw std::invalid_argument{"Unexpected EOF"};
        }
    }
    return res;
}

auto part1(const std::unique_ptr<Node>& root) -> int
{
    auto ownMetadata = std::ranges::fold_left(root->metadata, 0, std::plus{});
    return std::ranges::fold_left(root->children | std::views::transform(part1), ownMetadata,
                                  std::plus<>());
}

auto part2(const std::unique_ptr<Node>& root) -> int
{
    if (root->children.empty())
    {
        return std::ranges::fold_left(root->metadata, 0, std::plus{});
    }
    return std::ranges::fold_left(
        root->metadata |
            std::views::filter([maxIndex = std::ssize(root->children)](auto n)
                               { return n <= maxIndex && 0 < n; }) |
            std::views::transform([&root](auto n) { return part2(root->children[n - 1]); }),
        0, std::plus{});
}

} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto root = parseNode(input);

    return {part1(root), part2(root)};
}
} // namespace aoc
