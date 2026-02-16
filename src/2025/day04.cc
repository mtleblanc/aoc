#include <string>
#include <vector>

#include "aoc.hh"
namespace aoc
{
constexpr size_t YEAR = 2025;
constexpr size_t DAY = 4;
namespace
{
struct Grid
{
    std::vector<std::string> v;
    Grid(const std::vector<std::string>& v) : v{v} {}
    char border{'.'};
    [[nodiscard]] size_t width() const
    {
        return v[0].length();
    }
    [[nodiscard]] size_t height() const
    {
        return v.size();
    }
    char& operator()(size_t x, size_t y)
    {
        if (x >= width() || y >= height())
        {
            return border;
        }
        return v[y][x];
    }
};

bool canRemove(Grid& g, size_t x, size_t y)
{

    constexpr int MAX_ROLLS = 5;
    if (g(x, y) != '@')
    {
        return false;
    }
    int around{0};
    for (int xo = -1; xo < 2; xo++)
    {
        for (int yo = -1; yo < 2; yo++)
        {
            if (g(x + xo, y + yo) == '@')
            {
                around++;
            }
        }
    }
    return around < MAX_ROLLS;
}

ssize_t part1(Grid g)
{
    ssize_t canBeRemoved{};
    for (size_t x = 0; x < g.width(); x++)
    {
        for (size_t y = 0; y < g.height(); y++)
        {
            if (canRemove(g, x, y))
            {
                canBeRemoved++;
            }
        }
    }
    return canBeRemoved;
}

ssize_t update(Grid& g)
{
    ssize_t removed{};
    for (size_t x = 0; x < g.width(); x++)
    {
        for (size_t y = 0; y < g.height(); y++)
        {
            if (canRemove(g, x, y))
            {
                g(x, y) = '.';
                removed++;
            }
        }
    }
    return removed;
}

ssize_t part2(Grid g)
{
    ssize_t accum{0};
    ssize_t removed{};
    while ((removed = update(g)) > 0)
    {
        accum += removed;
    }
    return accum;
}
} // namespace
template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    auto lines = readAllLines(input);
    Grid g{lines};
    return SsizeSolution{part1(g), part2(g)};
}

} // namespace aoc
