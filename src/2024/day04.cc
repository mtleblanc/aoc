#include "aoc.hh"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <limits>
#include <utility>

/* https://adventofcode.com/2024/day/4
 *
 * Input:
 * XAMS
 * MASX
 * ...
 *
 * Part 1: Count occurrences of XMAS in the wordsearch
 * Part 2: Count occurrences of 2 diagonal MAS intersecting at A
 */
namespace aoc
{
constexpr size_t YEAR = 2024;
constexpr size_t DAY = 4;

namespace
{
class WordSearch
{
  public:
    using size_type = std::string::size_type;
    using difference_type = std::string::difference_type;
    using const_iterator = std::string::const_iterator;
    using offset_type = std::pair<difference_type, difference_type>;
    // NB: newlines aren't removed, so width includes them.  This lets us use w for stride length and
    // doesn't affect searching since \n is just a mismatch like any other character
    explicit WordSearch(const std::string& s) : s{s}, w(s.find('\n') + 1), h(s.size() / w)
    {
        assert(w * h == s.size());
        assert(w < std::numeric_limits<difference_type>::max());
        assert(h < std::numeric_limits<difference_type>::max());
    }
    [[nodiscard]] size_type width() const
    {
        return w;
    }
    [[nodiscard]] size_type height() const
    {
        return h;
    }
    [[nodiscard]] difference_type dwidth() const
    {
        return static_cast<difference_type>(w);
    }
    [[nodiscard]] difference_type dheight() const
    {
        return static_cast<difference_type>(h);
    }

    [[nodiscard]] char operator()(size_type x, size_type y) const
    {
        return s[w * y + x];
    }

    [[nodiscard]] auto begin() const
    {
        return s.begin();
    }
    [[nodiscard]] auto end() const
    {
        return s.end();
    }

    [[nodiscard]] bool matchAt(const_iterator it, offset_type o, const std::string& target) const
    {
        if (!canMove(it, o, static_cast<difference_type>(target.size()) - 1))
        {
            return false;
        }
        auto stride = o.first + dwidth() * o.second;

        for (auto c : target)
        {
            if (c != *it)
            {
                return false;
            }
            it += stride;
        }
        return true;
    }

  private:
    std::string s;
    size_type w;
    size_type h;

    [[nodiscard]] bool canMove(const_iterator it, offset_type o, difference_type steps) const
    {
        auto idx = (it - s.begin());
        auto r = static_cast<difference_type>(idx / w);
        auto c = static_cast<difference_type>(idx % w);
        auto r2 = r + o.second * steps;
        auto c2 = c + o.first * steps;
        return r2 >= 0 && r2 < static_cast<difference_type>(h) && c2 >= 0 &&
               c2 < static_cast<difference_type>(w);
    }
};

constexpr auto kingMoves()
{
    constexpr size_t NUM_MOVES = 8;
    std::array<WordSearch::offset_type, NUM_MOVES> deltas;
    size_t i{0};
    for (ssize_t dx{-1}; dx <= 1; ++dx)
    {
        for (ssize_t dy{-1}; dy <= 1; ++dy)
        {
            if (dx != 0 || dy != 0)
            {
                deltas[i++] = {dx, dy};
            }
        }
    }
    return deltas;
}

constexpr auto diagonalMoves()
{
    constexpr size_t NUM_MOVES = 4;
    std::array<WordSearch::offset_type, NUM_MOVES> deltas;
    size_t i{0};
    for (ssize_t dx : {-1, 1})
    {
        for (ssize_t dy : {-1, 1})
        {
            deltas[i++] = {dx, dy};
        }
    }
    return deltas;
}

int part1(const WordSearch& ws)
{
    constexpr std::string TARGET = "XMAS";
    int accum{};
    constexpr auto DELTAS = kingMoves();

    for (auto it = ws.begin(); it != ws.end(); ++it)
    {
        for (auto o : DELTAS)
        {
            if (ws.matchAt(it, o, TARGET))
            {
                ++accum;
            }
        }
    }
    return accum;
}

int part2(const WordSearch& ws)
{
    constexpr auto DELTAS = diagonalMoves();
    int accum{};
    for (size_t y{1}; y < ws.height() - 1; ++y)
    {
        for (size_t x{1}; x < ws.width() - 1; ++x)
        {
            if (ws(x, y) != 'A')
            {
                continue;
            }
            auto isMS = [&](auto delta)
            {
                auto [dx, dy] = delta;
                auto c = ws(x + dx, y + dy);
                return c == 'M' || c == 'S';
            };
            if (!std::ranges::all_of(DELTAS, isMS))
            {
                continue;
            }
            if ((ws(x - 1, y - 1) != ws(x + 1, y + 1)) && (ws(x - 1, y + 1) != ws(x + 1, y - 1)))
            {
                ++accum;
            }
        }
    }
    return accum;
}
} // namespace

template <> Solution_t<YEAR,DAY> solve<YEAR, DAY>(std::istream& input)
{
    WordSearch ws{slurp(input)};
    return {part1(ws), part2(ws)};
}
} // namespace aoc
