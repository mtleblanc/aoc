#include "aoc.hh"
#include <algorithm>
#include <ctre.hpp>
#include <iostream>
#include <map>
#include <ranges>

/* https://adventofcode.com/2016/day/4
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 4;

namespace
{
struct Room
{
    std::string name;
    ssize_t id{};
    std::string checksum;

    [[nodiscard]] bool isValid() const
    {
        constexpr ssize_t CHECKSUM_LENGTH = 5;
        if (checksum.size() != CHECKSUM_LENGTH)
        {
            return false;
        }

        std::map<char, int> counts;
        std::ranges::for_each(name | std::views::filter([](auto c) { return c != '-'; }),
                              [&counts](auto c) { counts[c]++; });
        auto revPair = [](auto p)
        {
            auto [c, n] = p;
            return std::make_pair(-n, c);
        };
        auto v = std::ranges::to<std::vector>(counts | std::views::transform(revPair));
        std::ranges::partial_sort(v, v.begin() + CHECKSUM_LENGTH);
        auto expected = v | std::views::take(CHECKSUM_LENGTH) |
                        std::views::transform(&std::pair<int, char>::second) |
                        std::ranges::to<std::string>();
        return expected == checksum;
    }

    [[nodiscard]] std::string decrypt() const
    {
        constexpr ssize_t PERIOD = 26;
        // assumes input is only [a-z\-]
        auto decryptLetter = [this](auto c) -> char
        {
            if (c == '-')
            {
                return ' ';
            }
            return (c - 'a' + this->id) % PERIOD + 'a';
        };
        return name | std::views::transform(decryptLetter) | std::ranges::to<std::string>();
    }
};

[[maybe_unused]] std::istream& operator>>(std::istream& is, Room& r)
{
    static constexpr auto PAT = ctll::fixed_string(R"(([a-z\-]+)-(\d+)\[([a-z]+)\])");

    std::string line;
    if (!std::getline(is, line))
    {
        is.setstate(std::ios_base::failbit);
        return is;
    }
    if (auto m = ctre::match<PAT>(line))
    {
        r.name = m.get<1>();
        r.id = std::stol(std::string(m.get<2>()));
        r.checksum = m.get<3>();
    }
    return is;
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto rooms = readAll<Room>(input);
    auto valid = rooms | std::views::filter(&Room::isValid);
    auto part1 =
        std::ranges::fold_left(valid | std::views::transform(&Room::id), 0L, std::plus<>());
    const std::string target = "northpole object storage";
    auto part2 = std::ranges::find(valid, target, &Room::decrypt)->id;
    return {part1, part2};
}
} // namespace aoc
