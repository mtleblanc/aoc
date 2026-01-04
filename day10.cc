#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <ranges>
#include <numeric>
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
    bool done = false;
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
                    done = true;
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
    const size_t len = p.target.size();
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

struct matrix
{
    size_t rows_, cols_;
    std::vector<float> data;
    matrix(size_t r, size_t c) : rows_(r), cols_(c), data(r * c, 0) {}
    explicit matrix(const problem &p)
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
    float &operator()(size_t r, size_t c) { return data[r * cols_ + c]; }
    const float &operator()(size_t r, size_t c) const { return data[r * cols_ + c]; }

    matrix *swap(size_t r1, size_t r2)
    {
        std::swap_ranges(
            data.begin() + r1 * cols_,
            data.begin() + (r1 + 1) * cols_,
            data.begin() + r2 * cols_);
        return this;
    }

    matrix *scale(size_t r, float s)
    {
        transform(
            data.begin() + r * cols_,
            data.begin() + (r + 1) * cols_,
            data.begin() + r * cols_,
            [s](float v)
            { return v * s; });
        return this;
    }

    matrix *add(size_t r1, size_t r2, float s)
    {
        transform(
            data.begin() + r2 * cols_,
            data.begin() + (r2 + 1) * cols_,
            data.begin() + r1 * cols_,
            data.begin() + r2 * cols_,
            [s](float v1, float v2)
            { return v1 + v2 * s; });
        return this;
    }

    matrix *rowReduce();
};

std::ostream &operator<<(std::ostream &os, const matrix &m)
{
    for (size_t r = 0; r < m.rows_; r++)
    {
        for (size_t c = 0; c < m.cols_; c++)
        {
            os << m(r, c) << " ";
        }
        os << std::endl;
    }
    return os;
}

matrix *matrix::rowReduce()
{
    size_t lead = 0;
    for (size_t r = 0; r < rows_; r++)
    {
        if (lead >= cols_)
            return this;
        size_t i = r;
        while (data[i * cols_ + lead] == 0)
        {
            i++;
            if (i == rows_)
            {
                i = r;
                lead++;
                if (lead == cols_)
                    return this;
            }
        }
        if (i != r)
        {
            swap(i, r);
            // std::cout << *this << std::endl;
        }

        float lv = data[r * cols_ + lead];
        if (lv != 1)
        {
            scale(r, 1.0f / lv);
            // std::cout << *this << std::endl;
        }
        lv = data[r * cols_ + lead];
        for (size_t i = 0; i < rows_; i++)
        {
            if (i != r)
            {
                float lv2 = data[i * cols_ + lead];
                float factor = lv2 / lv;
                if (factor != 0)
                {
                    add(r, i, -lv2 / lv);
                    // std::cout << *this << std::endl;
                }
            }
        }
        lead++;
    }
    return this;
}

uint64_t bfs2(problem &p)
{
    return 0;
    const size_t len = p.target.size();
    std::deque<node<int>> horizon;
    node<int> start{p.joltages, 0, 0};
    horizon.push_back(start);
    while (horizon.size() > 0)
    {
        node<int> cur = horizon.front();
        horizon.pop_front();
        for (; cur.idx < p.moves.size(); cur.idx++)
        {
            bool bad = false;
            node<int> next{cur.value, cur.idx, cur.cnt + 1};
            std::vector<size_t> move = p.moves[cur.idx];
            for (size_t m : move)
            {
                next.value[m] = next.value[m] - 1;
                if (next.value[m] < 0)
                    bad = true;
            }
            if (bad)
                continue;
            if (std::accumulate(next.value.begin(), next.value.end(), 0) == 0)
                return next.cnt;
            horizon.push_back(next);
        }
    }
    return 0;
}

size_t threes = 0;

uint64_t
part1(std::vector<problem> &v)
{
    uint64_t accum{};
    for (auto &p : v)
        accum += bfs1(p);
    return accum;
}

uint32_t minPresses(matrix &m)
{
    std::cout << m << std::endl;
    matrix orig{m};
    m.rowReduce();
    std::cout << m << std::endl;
    matrix rr{m};
    uint32_t presses = 0;
    std::vector<size_t> kernel_cols;
    size_t pivot_row = 0;
    for (size_t c = 0; c < m.cols_ - 1; c++)
    {
        if (m(pivot_row, c) == 1)
        {
            // This is a pivot column
            pivot_row++;
        }
        else
        {
            // This is a free variable
            kernel_cols.push_back(c);
        }
    }
    std::vector<float> deltas;
    for (size_t kc : kernel_cols)
    {
        float delta = 0;
        for (size_t r = 0; r < m.rows_; r++)
        {
            delta += m(r, kc);
        }
        deltas.push_back(delta);
    }
    for (size_t row = 0; row < m.rows_; row++)
    {
        restart:
        auto rhs = m(row, m.cols_ - 1);
        if (rhs < -0.1)
        {
            for (auto kc : kernel_cols)
            {
                float lhs = m(row, kc);
                if (lhs < -0.1)
                {
                    auto needed = static_cast<int64_t>(rhs / m(row, kc) + 0.9f);
                    for(size_t r3 = 0; r3 < row; r3++) {
                        if(m(r3, m.cols_ -1) - needed * m(r3, kc) < -0.1) goto next_kc;
                    }
                    assert (needed < 200);
                    presses += needed;
                    for (size_t r2 = 0; r2 < m.rows_; r2++)
                    {
                        m(r2, m.cols_ - 1) -= needed * m(r2, kc);
                    }
                    
                    assert(m(row, m.cols_ - 1) >= -0.1);
                    row = 0;
                    goto restart;
                }
                next_kc:
                continue;
            }
        }
    }

    return presses;
}

uint64_t part2(std::vector<problem> &v)
{
    uint64_t accum{};
    size_t cur = 0;
    for (auto &p : v)
    {
        matrix m{p};
        // std::cout << m << std::endl;
        accum += minPresses(m);
        cur++;
    }
    std::cout << threes << " problems had 3 free variables." << std::endl;
    return accum;
}

int main()
{
    std::ifstream fs{"input10.txt"};
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