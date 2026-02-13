#include "aoc.hh"
#include <algorithm>
#include <ctre.hpp>
#include <iostream>
#include <ranges>
#include <sstream>
#include <variant>

/* https://adventofcode.com/2016/day/8
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 8;

namespace
{
struct RotateRowCommand
{
    int row;
    int amount;
};

struct RotateColumnCommand
{
    int col;
    int amount;
};

struct RectCommand
{
    int rows;
    int columns;
};

using Command = std::variant<RectCommand, RotateRowCommand, RotateColumnCommand>;

[[maybe_unused]] std::istream& operator>>(std::istream& is, Command& com)
{
    std::string text;
    std::getline(is, text);

    static constexpr auto RECT = ctll::fixed_string(R"(rect (\d+)x(\d+))");
    static constexpr auto ROTATE_ROW = ctll::fixed_string(R"(rotate row y=(\d+) by (\d+))");
    static constexpr auto ROTATE_COLUMN = ctll::fixed_string(R"(rotate column x=(\d+) by (\d+))");

    if (auto m = ctre::match<RECT>(text))
    {
        com = RectCommand{.rows = m.get<2>().to_number(), .columns = m.get<1>().to_number()};
        if (std::get<RectCommand>(com).rows <= 0 || std::get<RectCommand>(com).columns <= 0)
        {
            throw std::invalid_argument("Rectangle dimensions must be positive");
        }
        return is;
    }
    if (auto m = ctre::match<ROTATE_ROW>(text))
    {
        com = RotateRowCommand{.row = m.get<1>().to_number(), .amount = m.get<2>().to_number()};
        if (std::get<RotateRowCommand>(com).amount <= 0)
        {
            throw std::invalid_argument("Rotate amount must be positive");
        }
        return is;
    }
    if (auto m = ctre::match<ROTATE_COLUMN>(text))
    {
        com = RotateColumnCommand{.col = m.get<1>().to_number(), .amount = m.get<2>().to_number()};
        if (std::get<RotateColumnCommand>(com).amount <= 0)
        {
            throw std::invalid_argument("Rotate amount must be positive");
        }
        return is;
    }
    is.setstate(std::ios_base::failbit);
    return is;
}

[[maybe_unused]] std::ostream& operator<<(std::ostream& os,
                                          const std::vector<std::vector<char>>& grid)
{
    for (const auto& row : grid)
    {
        for (auto bit : row)
        {
            os << (bit ? '#' : '.');
        }
        os << '\n';
    }
    return os;
}

template <class... Ts> struct Overloaded : Ts...
{
    using Ts::operator()...;
};

template <typename T> struct ColumnIterator
{
    using difference_type = int;
    using value_type = T;
    using reference = T&;
    std::vector<std::vector<T>>* base;
    int column{};
    int row{};

    ColumnIterator& operator++()
    {
        ++row;
        return *this;
    }

    ColumnIterator operator++(int)
    {
        auto prev = *this;
        ++*this;
        return prev;
    }

    bool operator==(const ColumnIterator& o) const
    {
        return row == o.row;
    }

    reference operator*() const
    {
        return (*base)[row][column];
    }
};

template <int ROWS, int COLS> StringSolution part1(const std::vector<Command>& commands)
{
    std::vector<std::vector<char>> grid(ROWS, std::vector<char>(COLS));
    auto rect = [&grid](RectCommand c)
    {
        for (auto row : std::views::iota(0, c.rows))
        {
            for (auto col : std::views::iota(0, c.columns))
            {
                grid[row][col] = 1;
            }
        }
    };
    auto rotateCol = [&grid](RotateColumnCommand c)
    {
        std::ranges::rotate(ColumnIterator{.base = &grid, .column = c.col, .row = 0},
                            ColumnIterator{.base = &grid, .column = c.col, .row = ROWS - c.amount},
                            ColumnIterator{.base = &grid, .column = c.col, .row = ROWS});
    };
    auto rotateRow = [&grid](RotateRowCommand c)
    { std::ranges::rotate(grid[c.row], std::next(grid[c.row].begin(), COLS - c.amount)); };
    for (const auto& c : commands)
    {
        std::visit(Overloaded{rect, rotateCol, rotateRow}, c);
    }
    // Part 2
    std::ostringstream oss;
    oss << grid;
    return {std::to_string(std::ranges::fold_left(
        grid | std::views::transform([](auto r) { return std::ranges::count(r, 1); }), 0,
        std::plus<>())), oss.str()};
}
} // namespace

template<> struct SolutionType<YEAR, DAY> {
    using type = StringSolution;
};

template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    constexpr int ROWS = 6;
    constexpr int COLS = 50;
    auto commands = readAll<Command>(input);
    // part2 is just printed out during part1, SsizeSolution doesn't take strings anyway
    return {part1<ROWS, COLS>(commands)};
}
} // namespace aoc
