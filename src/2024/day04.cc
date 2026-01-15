#include "aoc.hh"
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
 * Part 1:
 */
namespace aoc
{
constexpr size_t YEAR = 2024;
constexpr size_t DAY = 4;

namespace
{
class WordSearch
{
    static constexpr char DUMMY = '.';
    using size_type = std::string::size_type;
    using difference_type = std::string::difference_type;
    std::string s;
    size_type w;
    size_type h;

  public:
    WordSearch(const std::string& s) : s{s}, w(s.find('\n')), h(s.size() / w)
    {
        assert(w * h == s.size());
        assert(w < std::numeric_limits<int>::max());
        assert(h < std::numeric_limits<int>::max());
    }
    [[__nodiscard__]] size_type width() const
    {
        return w;
    }
    [[__nodiscard__]] size_type height() const
    {
        return h;
    }
    [[__nodiscard__]] difference_type dwidth() const
    {
        return static_cast<difference_type>(w);
    }
    [[__nodiscard__]] difference_type dheight() const
    {
        return static_cast<difference_type>(h);
    }
    // Return character at grid location, with dummy return for locations outside of grid
    // arguments are size_type, attempts to use negatives will wrap around and correctly fail the
    // range check
    [[__nodiscard__]] char operator()(size_type x, size_type y) const
    {
        if (x >= w || y >= h)
        {
            return DUMMY;
        }
        return s[h * y + x];
    }

    [[nodiscard]] auto begin() const
    {
        return s.begin();
    }
    [[nodiscard]] auto end() const
    {
        return s.end();
    }

    [[__nodiscard__]] bool matchAt(size_t x, size_t y, ssize_t dx, ssize_t dy,
                                   const std::string& target) const
    {
        s.begin();
        for (auto c : target)
        {
            if (c != (*this)(x, y))
            {
                return false;
            }
            x += dx;
            y += dy;
        }
        return true;
    }
};

template <class It> class TwoDimGuardedIterator : public It
{
    using iterator_category = std::random_access_iterator_tag;
    using value_type = It::value_type;
    using difference_type = It::difference_type;
    using pointer = It::pointer;
    using reference = It::reference;

    difference_type stride;
    It start;
    It end_;
    bool oob{false};
    value_type oobValue;

  public:
    TwoDimGuardedIterator(It start, difference_type rows, difference_type columns,
                          value_type oobValue)
        : It{start}, stride{columns}, start{start}, end_{start + rows * columns}, oobValue(oobValue)
    {
    }

    [[nodiscard]] TwoDimGuardedIterator<It> end() const
    {
        return TwoDimGuardedIterator<It>{end_, 0, 0, oobValue};
    }
    reference operator*()
    {
        if (oob)
        {
            return oobValue;
        }
        return It::operator*();
    }

    pointer operator->()
    {
        if (oob)
        {
            return oobValue;
        }
        return It::operator->();
    }

    [[__nodiscard__]] difference_type row() const
    {
        return (*this - start) / stride;
    }
    [[__nodiscard__]] difference_type col() const
    {
        return (*this - start) % stride;
    }
    auto& operator+=(difference_type d)
    {
        return this->It::operator+=(d);
    }
    auto& operator+=(std::pair<difference_type, difference_type> delta)
    {
        auto newCol = col() + delta.second;
        if (newCol < 0 || newCol >= stride)
        {
            oob = true;
        }
        *this += delta.first * stride + delta.second;
        if (*this < start || *this >= end_)
        {
            oob = true;
        }
        return *this;
    }
};

template <class It>
TwoDimGuardedIterator<It>
operator+(TwoDimGuardedIterator<It> it,
          std::pair<typename It::difference_type, typename It::difference_type> delta)
{
    return it += delta;
}

size_t part1(const WordSearch& ws)
{
    constexpr std::string TARGET = "XMAS";
    size_t accum{};
    // TwoDimGuardedIterator wsi{ws.begin(), ws.dheight(), ws.dwidth(), '.'};

    // auto wsend = wsi.end();
    std::vector<std::pair<ssize_t, ssize_t>> deltas;
    for (ssize_t dx{-1}; dx <= 1; ++dx)
    {
        for (ssize_t dy{-1}; dy <= 1; ++dy)
        {
            if (dx != 0 || dy != 0)
            {
                deltas.emplace_back(dx, dy);
            }
        }
    }

    // for (; wsi != wsend; ++wsi)
    // {
    //     for (auto del : deltas)
    //     {
    //         auto twsi = wsi;
    //         bool matching{true};
    //         for (auto c : TARGET)
    //         {
    //             if (c != *twsi)
    //             {
    //                 matching = false;
    //                 break;
    //             }
    //             twsi += del;
    //         }
    //         if (matching)
    //         {
    //             ++accum;
    //         }
    //     }
    // }

    for (size_t x{0}; x < ws.width(); ++x)
    {
        for (size_t y{0}; y < ws.height(); ++y)
        {
            if (ws(x, y) != TARGET[0])
            {
                continue;
            }
            for (auto [dx, dy] : deltas)
            {
                if (ws.matchAt(x, y, dx, dy, TARGET))
                {
                    ++accum;
                }
            }
        }
    }
    return accum;
}

size_t part2(const WordSearch& ws)
{
    std::vector<std::pair<ssize_t, ssize_t>> deltas;
    for (ssize_t dx : {-1, 1})
    {
        for (ssize_t dy : {-1, 1})
        {
            if (dx != 0 && dy != 0)
            {
                deltas.emplace_back(dx, dy);
            }
        }
    }
    size_t accum{};
    for (size_t y{0}; y < ws.height(); ++y)
    {
        for (size_t x{0}; x < ws.width(); ++x)
        {
            if (ws(x, y) != 'A')
            {
                continue;
            }
            for (auto [dx, dy] : deltas)
            {
                auto c = ws(x + dx, y + dy);
                if (c != 'M' && c != 'S')
                {
                    goto cont;
                }
            }
            if ((ws(x - 1, y - 1) != ws(x + 1, y + 1)) && (ws(x - 1, y + 1) != ws(x + 1, y - 1)))
            {
                ++accum;
            }
        cont:;
        }
    }
    return accum;
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    WordSearch ws{slurp(input)};
    return {part1(ws), part2(ws)};
}
} // namespace aoc
