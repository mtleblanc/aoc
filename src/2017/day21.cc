#include "aoc.hh"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <limits>
#include <ranges>
#include <stdexcept>

/* https://adventofcode.com/2017/day/21
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 21;
using Solution = Solution_t<YEAR, DAY>;

namespace
{

struct Square
{
    std::vector<int> elems_;
    int size_;

    Square(int size = 0)
        : elems_(static_cast<std::vector<int>::size_type>(size * size)), size_{size}
    {
    }

    template <int N> static Square of(int index)
    {
        auto res = Square(N);
        int i = N * N;
        while (i-- > 0)
        {
            res.elems_[i] = index & 1;
            index >>= 1;
        }
        return res;
    }

    int& operator[](int x, int y)
    {
        return elems_[y * size_ + x];
    }

    const int& operator[](int x, int y) const
    {
        return elems_[y * size_ + x];
    }

    void copyFrom(const Square& o, int x, int y)
    {
        if (x < 0 || x + o.size_ > size_ || y < 0 || y + o.size_ > size_)
        {
            throw std::out_of_range("Source square out of bounds");
        }
        for (int dy = 0; dy < o.size_; ++dy)
        {
            for (int dx = 0; dx < o.size_; ++dx)
            {
                (*this)[x + dx, y + dy] = o[dx, dy];
            }
        }
    }

    void copyTo(Square& o, int x, int y) const
    {
        if (x < 0 || x + o.size_ > size_ || y < 0 || y + o.size_ > size_)
        {
            throw std::out_of_range("Source square out of bounds");
        }
        for (int dy = 0; dy < o.size_; ++dy)
        {
            for (int dx = 0; dx < o.size_; ++dx)
            {
                o[dx, dy] = (*this)[x + dx, y + dy];
            }
        }
    }

    [[nodiscard]] Square subsquare(int size, int x, int y) const
    {
        Square res{size};
        copyTo(res, x, y);
        return res;
    }

    [[nodiscard]] Square transpose() const
    {
        Square res{size_};
        for (int y = 0; y < size_; ++y)
        {
            for (int x = 0; x < size_; ++x)
            {
                res[y, x] = (*this)[x, y];
            }
        }
        return res;
    }

    [[nodiscard]] Square flip() const
    {
        Square res{size_};
        for (int y = 0; y < size_; ++y)
        {
            for (int x = 0; x < size_; ++x)
            {
                res[x, size_ - y - 1] = (*this)[x, y];
            }
        }
        return res;
    }

    [[nodiscard]] int index() const
    {
        if (size_ * size_ >= std::numeric_limits<int>::digits)
        {
            throw std::domain_error("Square size too large for int index");
        }
        return std::ranges::fold_left(elems_, 0, [](auto acc, auto n) { return (acc << 1) | n; });
    }
};

struct TransitionRules
{
    // NOLINTBEGIN(*-magic-numbers)
    std::array<Square, 1 << 4> evens{};
    std::array<Square, 1 << 9> odds{};
    // NOLINTEND(*-magic-numbers)

    TransitionRules() = default;

    void parseRule(std::string_view sv)
    {
        auto rows = std::ranges::to<std::vector>(
            sv | std::views::split('>') |
            std::views::transform([](auto sr)
                                  { return static_cast<int>(std::ranges::count(sr, '/')) + 1; }));
        auto parseSquare = [](auto sr)
        {
            return std::ranges::to<std::vector>(
                sr | std::views::filter([](auto c) { return c == '.' || c == '#'; }) |
                std::views::transform([](auto c) { return c == '#' ? 1 : 0; }));
        };
        auto vals = std::ranges::to<std::vector>(sv | std::views::split('>') |
                                                 std::views::transform(parseSquare));

        assert(rows.size() == 2);
        assert(rows[0] == 2 || rows[0] == 3);
        assert(rows[0] + 1 == rows[1]);
        Square source{rows[0]};
        assert(vals[0].size() == source.elems_.size());
        source.elems_ = std::move(vals[0]);
        Square dest{rows[1]};
        assert(vals[1].size() == dest.elems_.size());
        dest.elems_ = std::move(vals[1]);

        std::vector<int> indices;
        for (int rotations = 0; rotations < 4; ++rotations)
        {
            indices.push_back(source.index());
            source = source.flip();
            indices.push_back(source.index());
            source = source.transpose();
        }

        if (source.size_ == 2)
        {
            for (auto i : indices)
            {
                evens[i] = dest;
            }
        }
        else
        {
            for (auto i : indices)
            {
                odds[i] = dest;
            }
        }
    }

    Square transform(const Square& sq)
    {
        if (sq.size_ % 2 == 0)
        {
            Square res{sq.size_ / 2 * 3};
            for (int y = 0; y < sq.size_ / 2; ++y)
            {
                for (int x = 0; x < sq.size_ / 2; ++x)
                {
                    auto source = sq.subsquare(2, x * 2, y * 2).index();
                    res.copyFrom(evens[source], 3 * x, 3 * y);
                }
            }
            return res;
        }

        if (sq.size_ % 3 == 0)
        {
            Square res{sq.size_ / 3 * 4};
            for (int y = 0; y < sq.size_ / 3; ++y)
            {
                for (int x = 0; x < sq.size_ / 3; ++x)
                {
                    auto source = sq.subsquare(3, x * 3, y * 3).index();
                    res.copyFrom(odds[source], 4 * x, 4 * y);
                }
            }
            return res;
        }

        throw std::invalid_argument("Squares size should be multiple of 2 or 3");
    }
};
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    static constexpr auto SEED = 0b010001111;
    static constexpr auto REPS1 = 5;
    static constexpr auto REPS2 = 18;
    auto lines = readAllLines(input);
    TransitionRules tr;
    for (const auto& l : lines)
    {
        tr.parseRule(l);
    }
    auto state = Square::of<3>(SEED);
    for (int i = 0; i < REPS1; ++i)
    {
        state = tr.transform(state);
    }
    auto part1 = std::ranges::fold_left(state.elems_, 0, std::plus<>());
    // TODO: This is plenty fast for REPS2 = 18, but we could go much further by noticing that after
    // the size goes 3->4->6->9, the 9 sub 3x3s are completely indepenedent.  We can just track how
    // many of each type there are and repeatedly move 3 iterations forward
    for (int i = REPS1; i < REPS2; ++i)
    {
        state = tr.transform(state);
    }
    auto part2 = std::ranges::fold_left(state.elems_, 0, std::plus<>());
    return {part1, part2};
}
} // namespace aoc
