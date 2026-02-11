#include "aoc.hh"
#include <algorithm>
#include <ranges>

/* https://adventofcode.com/2015/day/18
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 18;
namespace
{

// Seems to be fastest with uint64_t, but difference is negligible, might matter if grid size were
// larger.  Use at least uint16_t so that count() doesn't need to cast
using Cell = int64_t;

struct LifeGrid
{
  private:
    size_t storedRows;
    ssize_t stride;
    std::vector<Cell> grid;
    // keep buffer to avoid allocating every update()
    std::vector<Cell> buf;

  public:
    // grid has an extra 3 edges of padding.  the vertical padding (created by " " + l below) works
    // as both a left and right border as it sits between the nth rightmost cell and the n+1st
    // leftmost cell.  We put it first so that their coordiates are y*stride
    // i.e. &grid[(n+1)*stride] == &at(-1, n) == &at(cols(),n-1)
    // one extra cell is included at the end because at(cols(), rows()) is accessed when summing
    // neighbors of at(cols()-1, rows()-1)
    LifeGrid(const std::vector<std::string>& lines)
        : storedRows{lines.size() + 2}, stride{static_cast<ssize_t>(lines.front().size() + 1)}
    {
        using namespace std::views;
        auto gridView =
            lines |
            transform([](const auto& l)
                      { return ("." + l) | transform([](auto c) { return c == '#' ? 1 : 0; }); });

        grid.assign(stride, 0);
        std::ranges::copy(gridView | join, std::back_inserter(grid));
        grid.resize(grid.size() + stride + 1);
        buf = std::vector<Cell>(grid.size());
    }

    [[nodiscard]] ssize_t rows() const
    {
        return static_cast<ssize_t>(storedRows) - 2;
    }
    [[nodiscard]] ssize_t cols() const
    {
        return static_cast<ssize_t>(stride) - 1;
    }

    Cell& operator()(ssize_t x, ssize_t y)
    {
        return at(x, y);
    }

    Cell& at(ssize_t x, ssize_t y)
    {
        return grid[indexAt(x, y)];
    }

    [[nodiscard]] ssize_t indexAt(ssize_t x, ssize_t y) const
    {
        return x + 1 + (y + 1) * static_cast<ssize_t>(stride);
    }

    [[nodiscard]] ssize_t beginIndex() const
    {
        return indexAt(0, 0);
    }

    [[nodiscard]] ssize_t endIndex() const
    {
        return indexAt(cols() - 1, rows() - 1) + 1;
    }

    /**
     * Update grid according to Conway's Game of Life rules
     *
     * Uses a second buffer grid and swaps
     *
     * Instead of the usual split into living and dead cases, we count the cell itself and get the
     * simpler rule 3 total = live, 4 total = no change, other total = dead which can be done
     * branchlessly with some bit fiddling
     */
    void update()
    {
        auto end = endIndex();

        for (auto i{beginIndex()}; i < end; ++i)
        {
            // We could skip padding cells, but it's faster to avoid the branch and reset them after
            // if(i%stride != 0) continue;
            Cell sum{};
            for (ssize_t y{-1}; y <= 1; ++y)
            {
                for (ssize_t x{-1}; x <= 1; ++x)
                {
                    sum += grid[i + x + y * stride];
                }
            }
            buf[i] = (sum == 3) | (sum == 4 && grid[i]);
        }
        std::swap(buf, grid);
        for (ssize_t y{0}; y < rows(); ++y)
        {
            at(-1, y) = 0;
        }
    }

    void stickCorners()
    {
        at(0, 0) = 1;
        at(0, rows() - 1) = 1;
        at(cols() - 1, 0) = 1;
        at(cols() - 1, rows() - 1) = 1;
    }

    [[nodiscard]] Cell count() const
    {
        return std::ranges::fold_left(grid, 0, std::plus<>());
    }
};

} // namespace

template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    constexpr ssize_t ITERATIONS = 100;
    auto lines = readAllLines(input);
    LifeGrid life{lines};
    LifeGrid sticky{life};
    for (ssize_t i{}; i < ITERATIONS; ++i)
    {
        life.update();
    }
    sticky.stickCorners();
    for (ssize_t i{}; i < ITERATIONS; ++i)
    {
        sticky.update();
        sticky.stickCorners();
    }
    using std::ranges::fold_left;
    return {life.count(), sticky.count()};
}
} // namespace aoc
