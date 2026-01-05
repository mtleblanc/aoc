#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

struct problem
{
    std::vector<bool> target;
    std::vector<std::vector<size_t>> moves;
    std::vector<int> joltages;
};

std::istream &operator>>(std::istream &is, problem &p)
{
    char c;
    is >> c;
    if (!is || c != '[')
        throw std::invalid_argument("Malformed input");
    p.target.clear();
    for (bool done = false; !done;)
    {
        if (!(is >> c))
            throw std::invalid_argument("Malformed input");
        switch (c)
        {
        case '.':
            p.target.push_back(false);
            break;
        case '#':
            p.target.push_back(true);
            break;
        case ']':
            done = true;
            break;
        default:
            throw std::invalid_argument("Malformed input");
        }
    }
    p.moves.clear();
    while (is >> c)
    {
        if (c == ' ')
            continue;
        if (c == '(')
        {
            p.moves.emplace_back();
            for (;;)
            {
                p.moves.back().emplace_back();
                is >> p.moves.back().back();
                is >> c;
                if (c == ')')
                    break;
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
template <typename T>
struct node
{
    std::vector<T> value;
    size_t idx;
    size_t cnt;
};

uint64_t bfs1(problem &p)
{
    std::deque<node<bool>> horizon;
    node<bool> start{p.target, 0, 0};
    horizon.push_back(start);
    while (horizon.size() > 0)
    {
        node<bool> cur = horizon.front();
        horizon.pop_front();
        for (; cur.idx < p.moves.size(); cur.idx++)
        {
            node<bool> next{cur.value, cur.idx + 1, cur.cnt + 1};
            std::vector<size_t> move = p.moves[cur.idx];
            for (size_t m : move)
            {
                next.value[m] = !next.value[m];
            }
            if (std::find(next.value.begin(), next.value.end(), true) == next.value.end())
                return next.cnt;
            horizon.push_back(next);
        }
    }
    return 0;
}

template <typename T = float>
struct Matrix
{
    struct Column;
    struct Row;
    friend struct Column;
    friend struct Row;
    friend struct columnIterator;
    template <typename U>
    friend std::ostream &operator<<(std::ostream &, const Matrix<U> &);

private:
    size_t rows_, cols_;
    std::vector<T> data;

public:
    Matrix(size_t r, size_t c) : rows_(r), cols_(c), data(r * c, 0) {}
    explicit Matrix(const problem &p)
    {
        rows_ = p.joltages.size();
        cols_ = p.moves.size() + 1;
        data.resize(rows_ * cols_, 0);
        for (size_t r = 0; r < rows_; r++)
        {
            data[r * cols_ + cols_ - 1] = p.joltages[r];
        }

        for (size_t c = 0; c < cols_ - 1; c++)
        {
            for (size_t r : p.moves[c])
                data[r * cols_ + c] = 1;
        }
    }
    auto &operator()(size_t r, size_t c) { return data[r * cols_ + c]; }
    const auto &operator()(size_t r, size_t c) const { return data[r * cols_ + c]; }

    auto &swapRows(size_t r1, size_t r2)
    {
        std::swap_ranges(
            row(r1).begin(),
            row(r1).end(),
            row(r2).begin());
        return *this;
    }

    auto &scaleRow(size_t r, T s)
    {
        transform(
            row(r).begin(),
            row(r).end(),
            row(r).begin(),
            [s](T v)
            { return v * s; });
        return *this;
    }

    auto &addRows(size_t r1, size_t r2, T s)
    {
        transform(
            row(r1).begin(),
            row(r1).end(),
            row(r2).begin(),
            row(r2).begin(),
            [s](T v1, T v2)
            { return v1 * s + v2; });
        return *this;
    }

    auto rows() const { return rows_; }
    auto cols() const { return cols_; }
    auto row(size_t r)
    {
        return Row{r, *this};
    }
    auto column(size_t c)
    {
        return Column{c, *this};
    }

    Matrix &rowReduce();

    struct ColumnIterator : public std::iterator<
                                std::random_access_iterator_tag, // iterator_category
                                T,                               // value_type
                                ssize_t,                         // difference_type
                                const T *,                       // pointer
                                T                                // reference
                                >
    {
        friend struct Column;

    public:
        auto &operator*() { return matrix_(r, c); }
        auto &operator++()
        {
            r++;
            return *this;
        }
        auto &operator--()
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
        auto operator==(const ColumnIterator &other) const
        {
            return r == other.r;
        }
        auto operator!=(const ColumnIterator &other) const
        {
            return !(*this == other);
        }
        auto operator<(const ColumnIterator &other) const
        {
            return r < other.r;
        }
        auto &operator+=(size_t n)
        {
            r += n;
            return *this;
        }
        auto &operator-=(size_t n)
        {
            r -= n;
            return *this;
        }
        auto operator+(size_t n) const
        {
            return ColumnIterator{r + n, c, matrix_};
        }
        auto operator-(size_t n) const
        {
            return ColumnIterator{r - n, c, matrix_};
        }
        auto operator-(const ColumnIterator &other) const
        {
            return static_cast<ssize_t>(r) - static_cast<ssize_t>(other.r);
        }

    private:
        size_t r;
        size_t c;
        Matrix &matrix_;
        ColumnIterator(size_t r_, size_t c_, Matrix &m) : r(r_), c(c_), matrix_(m) {}
    };

    struct Column
    {
        size_t c;
        Matrix &matrix_;
        auto &operator[](size_t r) { return matrix_(r, c); }
        auto begin()
        {
            return ColumnIterator{0, c, matrix_};
        }
        auto end()
        {
            return ColumnIterator{matrix_.rows_, c, matrix_};
        }
    };

    struct Row
    {
        size_t r;
        Matrix &matrix_;
        auto &operator[](size_t c) { return matrix_(r, c); }
        auto begin()
        {
            return matrix_.data.begin() + r * matrix_.cols_;
        }
        auto end()
        {
            return matrix_.data.begin() + (r + 1) * matrix_.cols_;
        }
    };
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const Matrix<T> &m)
{
    for (size_t r = 0; r < m.rows_; r++)
    {
        for (size_t c = 0; c < m.cols_; c++)
        {
            os << std::setprecision(1) << std::setw(5) << std::setfill(' ')
               << std::fixed << m(r, c) << " ";
        }
        os << std::endl;
    }
    return os;
}

template <typename T>
Matrix<T> &Matrix<T>::rowReduce()
{
    size_t lead = 0;
    for (size_t r = 0; r < rows_; r++)
    {
        if (lead >= cols_)
            return *this;
        size_t i = r;
        while (data[i * cols_ + lead] == 0)
        {
            i++;
            if (i == rows_)
            {
                i = r;
                lead++;
                if (lead == cols_)
                    return *this;
            }
        }
        if (i != r)
        {
            swapRows(i, r);
        }

        T lv = data[r * cols_ + lead];
        if (lv != 1)
        {
            scaleRow(r, 1.0f / lv);
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

uint64_t
part1(std::vector<problem> &v)
{
    uint64_t accum{};
    for (auto &p : v)
        accum += bfs1(p);
    return accum;
}

template <typename T, typename It>
auto reduce_helper(std::vector<It> cols, std::vector<T> coeffs, std::vector<T> orig) {
    auto cli = cols.begin();
    auto cfi = coeffs.begin();
    for(; cli != cols.end(); ++cli, ++cfi) {
        std::transform(cli->begin(), cli->end(), orig.begin(), orig.begin(), [cfi](auto cv, auto ov) { return ov - cv * (*cfi); });
    }
}

template <typename T = float>
auto minPresses(Matrix<T> &m)
{
    size_t presses = SIZE_T_MAX;
    Matrix<T> orig{m};
    m.rowReduce();
    std::vector<size_t> free_column_indices;
    std::vector<typename Matrix<T>::Column> pivot_cols;
    size_t pivot_row = 0;
    for (size_t c = 0; c < m.cols() - 1; c++)
    {
        if (pivot_row < m.rows() && m(pivot_row, c) == 1)
        {
            pivot_cols.push_back(orig.column(c));
            pivot_row++;
        }
        else
        {
            free_column_indices.push_back(c);
        }
    }
    auto orig_target_col = orig.column(orig.cols() - 1);
    std::vector<T> orig_target{orig_target_col.begin(), orig_target_col.end()};
    auto rr_target_col = m.column(m.cols() - 1);
    std::vector<T> rr_target{rr_target_col.begin(), rr_target_col.end()};

    std::vector<T> free_coeffs(free_column_indices.size(), 0);
    std::vector<typename Matrix<T>::Column> free_columns_orig;
    std::vector<typename Matrix<T>::Column> free_columns_reduced;
    std::transform(free_column_indices.begin(), free_column_indices.end(), back_inserter(free_columns_orig), [&orig](auto c)
                   { return orig.column(c); });
    std::transform(free_column_indices.begin(), free_column_indices.end(), back_inserter(free_columns_reduced), [&m](auto c)
                   { return m.column(c); });
    // for (size_t index = 0;;)
    // {
        std::vector<T> trial_target{rr_target};
        reduce_helper(free_columns_reduced, free_coeffs, trial_target);
        if(true || std::all_of(trial_target.begin(), trial_target.end(), [](auto v){return v >= -0.1 && abs(v - (int)v) < 0.1;})) {
            std::transform(trial_target.begin(), trial_target.end(), trial_target.begin(), [](auto v){ return round(v); });
            std::for_each(trial_target.begin(), trial_target.end(), [](auto v){ std::cout << v << " ";});
            // break;
        }
        
    // }
    return presses;
}

uint64_t part2(std::vector<problem> &v)
{
    uint64_t accum{};
    for (auto &p : v)
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
    std::vector<problem> v;
    for (std::string s; getline(fs, s);)
    {
        if (s.length() == 0)
            break;
        std::istringstream iss{s};
        v.emplace_back();
        iss >> v.back();
    }

    std::cout << part1(v) << std::endl;
    std::cout << part2(v) << std::endl;
}