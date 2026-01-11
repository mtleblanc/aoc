#include <algorithm>
#include <cassert>
#include <climits>
#include <fstream>
#include <iostream>
#include <machine/limits.h>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include "matrix.hh"
#include "rational.hh"
using Q = Rational<int>;

struct Problem
{
    std::vector<bool> target;
    std::vector<std::vector<size_t>> moves;
    std::vector<int> joltages;
    template <typename T> [[nodiscard]] Matrix<T> toMatrix() const
    {
        Matrix<T> m{joltages.size(), moves.size() + 1};
        for (size_t r = 0; r < joltages.size(); r++)
        {
            m(r, m.cols() - 1) = static_cast<T>(joltages[r]);
        }
        for (size_t c = 0; c < moves.size(); c++)
        {
            for (size_t r : moves[c])
            {
                m(r, c) = 1;
            }
        }
        return m;
    }
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

uint64_t part1(std::vector<Problem>& v)
{
    uint64_t accum{};
    for (auto& p : v)
    {
        accum += bfs1(p);
    }
    return accum;
}
template <typename T, typename Coeff, typename Col>
auto reduceHelper(std::vector<Col>& cols, std::vector<Coeff>& coeffs, std::vector<T>& orig)
{
    auto cli = cols.begin();
    auto cfi = coeffs.begin();
    for (; cli != cols.end(); ++cli, ++cfi)
    {
        std::transform(cli->begin(), cli->end(), orig.begin(), orig.begin(),
                       [cfi](auto cv, auto ov) { return ov - cv * (*cfi); });
    }
}

template <typename T, typename Coeff, typename Col>
auto nextCoeffs(std::vector<Col>& cols, std::vector<Coeff>& coeffs, std::vector<T>& orig)
{
    for (size_t idx = 0; idx < coeffs.size(); idx++)
    {
        coeffs[idx]++;
        std::vector<T> test{orig};
        reduceHelper(cols, coeffs, test);
        if (std::all_of(test.begin(), test.end(), [](auto t) { return t >= 0; }))
        {
            return true;
        }
        coeffs[idx] = 0;
    }
    return false;
}

template <typename T = Q> auto minPresses(Matrix<T>& m)
{
    int presses = INT_MAX;
    Matrix<T> orig{m};
    // std::cout << m << std::endl;
    m.rowReduce();
    // std::cout << m << std::endl;
    size_t lastPivot = -1;
    size_t dim = std::min(m.rows(), m.cols() - 1);
    for (size_t c = 0; c < dim; c++)
    {
        if (m(c, c) == 1)
        {
            lastPivot = c;
            continue;
        }
        for (size_t d = c + 1; d < m.cols() - 1; d++)
        {
            if (m(c, d) == 1)
            {
                m.swapColumns(c, d);
                orig.swapColumns(c, d);
                lastPivot = c;
                break;
            }
        }
    }
    std::cout << m << std::endl;
    // std::cout << lastPivot << std::endl;

    std::vector<int> freeCoeffs((m.cols() - lastPivot - 2), 0);
    std::vector<typename Matrix<T>::Column> freeCols{};
    std::vector<typename Matrix<T>::Column> origFreeCols{};

    for (size_t c = lastPivot + 1; c < m.cols() - 1; c++)
    {
        freeCols.push_back(m.column(c));
        origFreeCols.push_back(orig.column(c));
    }

    std::vector<typename Matrix<T>::Column> allCols{};

    allCols.reserve(orig.cols() - 1);
    for (size_t c = 0; c < orig.cols() - 1; c++)
    {
        allCols.push_back(orig.column(c));
    };

    auto origTargetCol = orig.column(orig.cols() - 1);
    std::vector<T> origTarget{origTargetCol.begin(), origTargetCol.end()};
    auto rrTargetCol = m.column(m.cols() - 1);
    std::vector<T> rrTarget{rrTargetCol.begin(), rrTargetCol.end()};
    for (;;)
    {
        std::vector<T> rrTest{rrTarget};
        reduceHelper(freeCols, freeCoeffs, rrTest);
        if (std::all_of(rrTest.begin(), rrTest.end(), [](auto t) { return t >= 0; }))
        {
            std::vector<int> allCoeffs{};
            allCoeffs.reserve(m.cols() - 1);
            for (size_t c = 0; c < m.cols() - 1; c++)
            {
                allCoeffs.push_back(
                    static_cast<int>(c <= lastPivot ? rrTest[c] : freeCoeffs[c - lastPivot - 1]));
            }
            std::vector<T> origTest{origTarget};
            reduceHelper(allCols, allCoeffs, origTest);
            if (std::all_of(origTest.begin(), origTest.end(), [](auto t) { return t == 0; }))
            {
                presses = std::min(presses, std::accumulate(allCoeffs.begin(), allCoeffs.end(), 0));
            }
        }
        if (!nextCoeffs(origFreeCols, freeCoeffs, origTarget))
        {
            break;
        }
    }
    std::cout << presses << std::endl;
    return presses;
}

uint64_t part2(std::vector<Problem>& v)
{
    uint64_t accum{};
    // int count = 0;
    for (auto& p : v)
    {
        // std::cout << count++ << std::endl;
        auto m = p.toMatrix<Q>();
        accum += minPresses(m);
    }
    return accum;
}
#ifndef TESTING
int main()
{
    std::ifstream fs{"input10.txt"};
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
#endif