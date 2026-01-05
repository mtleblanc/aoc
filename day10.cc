#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

struct Problem
{
    std::vector<bool> target;
    std::vector<std::vector<size_t>> moves;
    std::vector<int> joltages;
};

std::vector<bool> readLights(std::istream& is)
{
    std::vector<bool> ret;
    char c{};
    is >> c;
    if (!is || c != '[')
    {
        throw std::invalid_argument("Malformed input");
    }
    for (bool done = false; !done;)
    {
        if (!(is >> c))
        {
            throw std::invalid_argument("Malformed input");
        }
        switch (c)
        {
        case '.':
            ret.push_back(false);
            break;
        case '#':
            ret.push_back(true);
            break;
        case ']':
            done = true;
            break;
        default:
            throw std::invalid_argument("Malformed input");
        }
    }
    return ret;
}

std::istream& operator>>(std::istream& is, Problem& p)
{
    p.target = readLights(is);
    p.moves.clear();
    char c{};
    while (is >> c)
    {
        if (c == ' ')
        {
            continue;
        }
        if (c == '(')
        {
            p.moves.emplace_back();
            for (;;)
            {
                p.moves.back().emplace_back();
                is >> p.moves.back().back();
                is >> c;
                if (c == ')')
                {
                    break;
                }
            }
        }
        if (c == '{')
        {
            p.joltages.clear();
            for (;;)
            {
                p.joltages.emplace_back();
                is >> p.joltages.back();
                is >> c;
                if (c == '}')
                {
                    break;
                }
            }
        }
    }
    return is;
}
template <typename T> struct Node
{
    std::vector<T> value;
    size_t idx;
    size_t cnt;
    Node(std::vector<T> value, size_t idx, size_t cnt)
        : value{std::forward<std::vector<T>>(value)}, idx{idx}, cnt{cnt}
    {
    }
};

uint64_t bfs1(Problem& p)
{
    std::deque<Node<bool>> horizon;
    Node<bool> start{p.target, 0, 0};
    horizon.push_back(start);
    while (horizon.size() > 0)
    {
        Node<bool> cur = horizon.front();
        horizon.pop_front();
        for (; cur.idx < p.moves.size(); cur.idx++)
        {
            Node<bool> next{cur.value, cur.idx + 1, cur.cnt + 1};
            std::vector<size_t> move = p.moves[cur.idx];
            for (size_t m : move)
            {
                next.value[m] = !next.value[m];
            }
            if (std::find(next.value.begin(), next.value.end(), true) == next.value.end())
            {
                return next.cnt;
            }
            horizon.push_back(next);
        }
    }
    return 0;
}

template <typename T = float> struct Matrix
{
    struct Column;
    struct Row;
    friend struct Column;
    friend struct Row;
    friend struct columnIterator;
    template <typename U> friend std::ostream& operator<<(std::ostream&, const Matrix<U>&);

  private:
    size_t rows_, cols_;
    std::vector<T> data;

  public:
    Matrix(size_t r, size_t c) : rows_(r), cols_(c), data(r * c, 0) {}
    explicit Matrix(const Problem& p) : rows_{p.joltages.size()}, cols_{p.moves.size() + 1}
    {
        data.resize(rows_ * cols_, 0);
        for (size_t r = 0; r < rows_; r++)
        {
            data[r * cols_ + cols_ - 1] = p.joltages[r];
        }

        for (size_t c = 0; c < cols_ - 1; c++)
        {
            for (size_t r : p.moves[c])
            {
                data[r * cols_ + c] = 1;
            }
        }
    }
    auto& operator()(size_t r, size_t c)
    {
        return data[r * cols_ + c];
    }
    const auto& operator()(size_t r, size_t c) const
    {
        return data[r * cols_ + c];
    }

    auto& swapRows(size_t r1, size_t r2)
    {
        std::swap_ranges(row(r1).begin(), row(r1).end(), row(r2).begin());
        return *this;
    }

    auto& scaleRow(size_t r, T s)
    {
        transform(row(r).begin(), row(r).end(), row(r).begin(), [s](T v) { return v * s; });
        return *this;
    }

    auto& addRows(size_t r1, size_t r2, T s)
    {
        transform(row(r1).begin(), row(r1).end(), row(r2).begin(), row(r2).begin(),
                  [s](T v1, T v2) { return v1 * s + v2; });
        return *this;
    }

    auto rows() const
    {
        return rows_;
    }
    auto cols() const
    {
        return cols_;
    }
    auto row(size_t r)
    {
        return Row{r, this};
    }
    auto column(size_t c)
    {
        return Column{c, this};
    }

    Matrix& rowReduce();

    struct ColumnIterator
    {
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = ssize_t;
        using pointer = T*;
        using reference = T&;
        friend struct Column;

      public:
        auto& operator*()
        {
            return (*matrix_)(r, c);
        }
        auto& operator[](difference_type i)
        {
            return (*matrix_)(r + i, c);
        }
        auto operator->()
        {
            return &(**this);
        }
        auto& operator++()
        {
            r++;
            return *this;
        }
        auto& operator--()
        {
            r--;
            return *this;
        }
        auto operator++(int)
        {
            ColumnIterator temp = *this;
            ++*this;
            return temp;
        }
        auto operator--(int)
        {
            ColumnIterator temp = *this;
            --*this;
            return temp;
        }
        auto operator==(const ColumnIterator& other) const
        {
            return r == other.r;
        }
        auto operator!=(const ColumnIterator& other) const
        {
            return !(*this == other);
        }
        auto operator<(const ColumnIterator& other) const
        {
            return r < other.r;
        }
        auto operator>(const ColumnIterator& other) const
        {
            return other < *this;
        }
        auto operator>=(const ColumnIterator& other) const
        {
            return !(*this < other);
        }
        auto operator<=(const ColumnIterator& other) const
        {
            return !(*this > other);
        }
        auto& operator+=(difference_type n)
        {
            r += n;
            return *this;
        }
        auto& operator-=(difference_type n)
        {
            r -= n;
            return *this;
        }
        auto operator+(difference_type n) const
        {
            return ColumnIterator{r + n, c, matrix_};
        }
        auto operator-(difference_type n) const
        {
            return ColumnIterator{r - n, c, matrix_};
        }
        auto operator-(const ColumnIterator& other) const
        {
            return static_cast<difference_type>(r) - static_cast<difference_type>(other.r);
        }
        friend ColumnIterator operator+(difference_type n, const ColumnIterator& it)
        {
            return it + n;
        }

      private:
        size_t r;
        size_t c;
        Matrix* matrix_;
        ColumnIterator(size_t r_, size_t c_, Matrix* m) : r(r_), c(c_), matrix_(m) {}
    };

    struct Column
    {
        size_t c;
        Matrix* matrix_;
        auto& operator[](size_t r)
        {
            return (*matrix_)(r, c);
        }
        auto begin()
        {
            return ColumnIterator{0, c, matrix_};
        }
        auto end()
        {
            return ColumnIterator{matrix_->rows_, c, matrix_};
        }
    };

    struct Row
    {
        size_t r;
        Matrix* matrix_;
        auto& operator[](size_t c)
        {
            return (*matrix_)(r, c);
        }
        auto begin()
        {
            return matrix_->data.begin() + r * matrix_->cols_;
        }
        auto end()
        {
            return matrix_->data.begin() + (r + 1) * matrix_->cols_;
        }
    };
};

template <typename T> std::ostream& operator<<(std::ostream& os, const Matrix<T>& m)
{
    for (size_t r = 0; r < m.rows_; r++)
    {
        for (size_t c = 0; c < m.cols_; c++)
        {
            // NOLINTNEXTLINE
            os << std::setprecision(1) << std::setw(5) << std::setfill(' ') << std::fixed << m(r, c)
               << " ";
        }
        os << std::endl;
    }
    return os;
}

template <typename T> Matrix<T>& Matrix<T>::rowReduce()
{
    size_t lead = 0;
    for (size_t r = 0; r < rows_; r++)
    {
        if (lead >= cols_)
        {
            return *this;
        }
        size_t i = r;
        while (data[i * cols_ + lead] == 0)
        {
            i++;
            if (i == rows_)
            {
                i = r;
                lead++;
                if (lead == cols_)
                {
                    return *this;
                }
            }
        }
        if (i != r)
        {
            swapRows(i, r);
        }

        T lv = data[r * cols_ + lead];
        if (lv != 1)
        {
            scaleRow(r, 1.0F / lv);
            // std::cout << *this << std::endl;
        }
        lv = data[r * cols_ + lead];
        for (size_t i = 0; i < rows_; i++)
        {
            if (i != r)
            {
                T lv2 = data[i * cols_ + lead];
                T factor = lv2 / lv;
                if (factor != 0)
                {
                    addRows(r, i, -lv2 / lv);
                    // std::cout << *this << std::endl;
                }
            }
        }
        lead++;
    }
    return *this;
}

uint64_t part1(std::vector<Problem>& v)
{
    uint64_t accum{};
    for (auto& p : v)
    {
        accum += bfs1(p);
    }
    return accum;
}

template <typename T, typename It>
auto reduceHelper(std::vector<It> cols, std::vector<T> coeffs, std::vector<T> orig)
{
    auto cli = cols.begin();
    auto cfi = coeffs.begin();
    for (; cli != cols.end(); ++cli, ++cfi)
    {
        std::transform(cli->begin(), cli->end(), orig.begin(), orig.begin(),
                       [cfi](auto cv, auto ov) { return ov - cv * (*cfi); });
    }
}

template <typename T = float> auto minPresses(Matrix<T>& m)
{
    size_t presses = SIZE_T_MAX;
    Matrix<T> orig{m};
    m.rowReduce();
    std::vector<size_t> freeColumnIdx;
    std::vector<typename Matrix<T>::Column> pivotCols;
    size_t pivotRow = 0;
    for (size_t c = 0; c < m.cols() - 1; c++)
    {
        if (pivotRow < m.rows() && m(pivotRow, c) == 1)
        {
            pivotCols.push_back(orig.column(c));
            pivotRow++;
        }
        else
        {
            freeColumnIdx.push_back(c);
        }
    }
    auto origTargetCol = orig.column(orig.cols() - 1);
    std::vector<T> origTarget{origTargetCol.begin(), origTargetCol.end()};
    auto rrTargetCol = m.column(m.cols() - 1);
    std::vector<T> rrTarget{rrTargetCol.begin(), rrTargetCol.end()};

    std::vector<T> coeffs(freeColumnIdx.size(), 0);
    std::vector<typename Matrix<T>::Column> freeColOrig;
    std::vector<typename Matrix<T>::Column> freeColRed;
    std::transform(freeColumnIdx.begin(), freeColumnIdx.end(), back_inserter(freeColOrig),
                   [&orig](auto c) { return orig.column(c); });
    std::transform(freeColumnIdx.begin(), freeColumnIdx.end(), back_inserter(freeColRed),
                   [&m](auto c) { return m.column(c); });
    // for (size_t index = 0;;)
    // {
    std::vector<T> trialTarget{rrTarget};
    reduceHelper(freeColRed, coeffs, trialTarget);
    constexpr auto EPS = 0.1F;
    if (true || std::all_of(trialTarget.begin(), trialTarget.end(), // NOLINT
                            [](auto v) { return v >= -EPS && abs(v - (int)v) < EPS; }))
    {
        std::transform(trialTarget.begin(), trialTarget.end(), trialTarget.begin(),
                       [](auto v) { return round(v); });
        std::for_each(trialTarget.begin(), trialTarget.end(),
                      [](auto v) { std::cout << v << " "; });
        // break;
    }

    // }
    return presses;
}

uint64_t part2(std::vector<Problem>& v)
{
    uint64_t accum{};
    for (auto& p : v)
    {
        Matrix<float> m{p};
        // std::cout << m << std::endl;
        accum += minPresses(m);
    }
    return accum;
}

int main()
{
    std::ifstream fs{"sample10.txt"};
    std::vector<Problem> v;
    for (std::string s; getline(fs, s);)
    {
        if (s.length() == 0)
        {
            break;
        }
        std::istringstream iss{s};
        v.emplace_back();
        try
        {
            iss >> v.back();
        }
        catch (const std::exception& ex)
        {
            std::cerr << ex.what() << std::endl;
            exit(1);
        }
        catch (...)
        {
            std::cerr << "Unknown exception" << std::endl;
        }
    }

    std::cout << part1(v) << std::endl;
    std::cout << part2(v) << std::endl;
}