#include "aoc.hh"
#include <algorithm>
#include <iostream>

/* https://adventofcode.com/2015/day/20
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 20;

namespace
{
struct SievePrime
{
    ssize_t next;
    ssize_t prime;

    SievePrime(ssize_t p) : next{p * 2}, prime{p} {}

    void advance()
    {
        next += prime;
    }

    auto operator<=>(const SievePrime& o) const = default;
};

/* Sieve of Eratosthenes
 *
 * Maintains a heap of the next multiple of each prime so that prime factors can be read off
 * Factors can be used to calculate d_1(n) efficiently
 */
struct Sieve
{
    std::vector<SievePrime> queue{SievePrime{2}};

    ssize_t n{2};
    std::vector<ssize_t> factors{2};
    bool isPrime = true;

    void next()
    {
        ++n;
        factors.clear();
        isPrime = false;
        for (SievePrime* s{}; (s = &queue.front())->next == n;)
        {
            factors.push_back(s->prime);
            s->advance();
            std::ranges::pop_heap(queue, std::greater<>());
            std::ranges::push_heap(queue, std::greater<>());
        }
        if (factors.empty())
        {
            isPrime = true;
            factors.push_back(n);
            queue.emplace_back(n);
            std::ranges::push_heap(queue, std::greater<>());
        }
    }

    // Calculate sum of divisors using the fact that if gcd(m,n)=1, d(mn) = d(m)d(n), and d(p^n) =
    // (p^(n+1) -1)/(p-1)
    ssize_t d1()
    {
        ssize_t d = 1;
        for (auto p : factors)
        {
            auto pn = p;
            while (n % (pn *= p) == 0)
            {
            }
            d *= (pn - 1) / (p - 1);
        }
        return d;
    }
};

// sums divisors of n whose quotient n/d <= LIM
template <ssize_t LIM> ssize_t d1Limited(ssize_t n)
{
    ssize_t d{};
    for (ssize_t e{1}; e <= LIM; ++e)
    {
        if (n % e == 0)
        {
            d += n / e;
        }
    }
    return d;
}

} // namespace

template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    constexpr ssize_t P1_MULT = 10;
    constexpr ssize_t P2_MULT = 11;
    constexpr ssize_t P2_LIM = 50;
    ssize_t target{};
    input >> target;
    ssize_t p1Target{(target + P1_MULT - 1) / P1_MULT};
    Sieve s;
    for (; s.d1() < p1Target; s.next())
    {
    }

    ssize_t p2Target{(target + P2_MULT - 1) / P2_MULT};
    ssize_t p2{p2Target / P2_LIM};
    for (; d1Limited<P2_LIM>(p2) < p2Target; ++p2)
    {
    }
    return {s.n, p2};
}
} // namespace aoc
