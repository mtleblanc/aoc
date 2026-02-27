#include "aoc.hh"
#include "util.hh"
#include <algorithm>
#include <cmath>
#include <queue>
#include <ranges>
#include <stdexcept>
#include <string>

/* https://adventofcode.com/2017/day/20
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 20;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
/* returns a vector of integer solutions to a*x^2 + b*x + c = 0, only non-negative unless
 * includeNegatives is specified
 *
 * The degenerate case of a=b=c=0 returns no solutions
 */
template <std::integral T>
constexpr auto solveQuadratic(const T a, const T b, const T c,
                              bool includeNegative = false) -> std::vector<T>
{
    if (a == 0)
    {
        // returns empty even if infinite solutions, up to caller to detect degeneracy
        if (b == 0)
        {
            return {};
        }
        if (auto r = -c / b; (includeNegative || r >= 0) && c % b == 0)
        {
            return {r};
        }
        return {};
    }
    auto discrim = b * b - 4 * a * c;
    if (discrim < 0)
    {
        return {};
    }
    auto sqrt = static_cast<T>(std::sqrt(discrim));
    if (sqrt * sqrt != discrim)
    {
        return {};
    }
    auto res = std::vector<T>{};
    auto n1 = -b + sqrt;
    if (auto r = n1 / (2 * a); (r >= 0 || includeNegative) && n1 % (2 * a) == 0)
    {
        res.push_back(r);
    }
    auto n2 = -b - sqrt;
    if (auto r = n2 / (2 * a); (r >= 0 || includeNegative) && n2 % (2 * a) == 0)
    {
        res.push_back(r);
    }
    return res;
}

using vec = std::array<int, 3>;
vec& operator+=(vec& self, const vec& o)
{
    for (auto [a, b] : std::views::zip(self, o))
    {
        a += b;
    }
    return self;
}

vec& operator-=(vec& self, const vec& o)
{
    for (auto [a, b] : std::views::zip(self, o))
    {
        a -= b;
    }
    return self;
}

vec operator-(vec l, const vec& r)
{
    return l -= r;
}

auto manhattan(auto v)
{
    return std::ranges::fold_left(v | std::views::transform([](auto c) { return std::abs(c); }), 0,
                                  std::plus<>());
};

template <size_t N, typename T> std::array<T, N> toArray(std::span<T> v)
{
    if (v.size() != N)
    {
        throw std::invalid_argument("Vector of length " + std::to_string(v.size()) +
                                    " cannot be assigned to array of size " + std::to_string(N));
    }
    std::array<T, N> res{};
    std::ranges::copy(v, res.begin());
    return res;
}

struct Particle
{
    vec p;
    vec v;
    vec a;
    std::optional<int> collisionTime;

    Particle(vec p, vec v, vec a) : p{p}, v{v}, a{a} {}
    void update()
    {
        v += a;
        p += v;
    }

    [[nodiscard]] Particle operator-(const Particle& o) const
    {
        return Particle{p - o.p, v - o.v, a - o.a};
    }

    [[nodiscard]] std::optional<int> firstZero() const
    {
        auto p0 = p[0];
        auto v0 = v[0];
        auto a0 = a[0];
        // This is not necessary on my input, but included for correctness.  Particles could always
        // have matching x coordinates but differ in y or z
        for (auto d = 1; d < 3 && p0 == 0 && v0 == 0 && a0 == 0; ++d)
        {
            p0 = p[d];
            v0 = v[d];
            a0 = a[d];
        }
        // degenerate case of stationary particle
        if (p0 == 0 && v0 == 0 && a0 == 0)
        {
            return 0;
        }
        auto checkDimension = [this](auto dim, auto t)
        {
            auto a = this->a[dim];
            auto v = this->v[dim];
            auto p = this->p[dim];
            return 2 * p + (2 * v + a) * t + a * t * t == 0;
        };
        auto check = [checkDimension](auto t)
        { return checkDimension(0, t) && checkDimension(1, t) && checkDimension(2, t); };

        auto solutions = solveQuadratic(a0, 2 * v0 + a0, 2 * p0);
        auto valid = std::ranges::to<std::vector>(solutions | std::views::filter(check));
        return valid.empty() ? std::nullopt : std::optional{std::ranges::min(valid)};
    }
};

struct Collision
{
    int time;
    std::pair<int, int> indices;
    Collision(int time, std::pair<int, int> indices) : time{time}, indices{indices} {}
    auto operator<=>(const Collision& o) const = default;
};

int part2(std::vector<Particle>& particles)
{
    auto collisions = std::priority_queue<Collision, std::vector<Collision>, std::greater<>>{};
    for (int i{}; i < std::ssize(particles); ++i)
    {
        for (int j{i + 1}; j < std::ssize(particles); ++j)
        {
            if (auto t = (particles[i] - particles[j]).firstZero())
            {
                collisions.emplace(*t, std::make_pair(i, j));
            }
        }
    }
    while (collisions.size())
    {
        auto collision = collisions.top();
        collisions.pop();
        auto t = collision.time;
        auto [l, r] = collision.indices;
        if (particles[l].collisionTime.value_or(t) >= t &&
            particles[r].collisionTime.value_or(t) >= t)
        {
            particles[l].collisionTime = t;
            particles[r].collisionTime = t;
        }
    }

    return static_cast<int>(
        std::ranges::count_if(particles, [](auto p) { return !p.collisionTime; }));
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto parse = [](auto line)
    {
#if __cpp_lib_ranges_chunk == 202202L
        auto coords = std::ranges::to<std::vector>(readNumbers(line) | std::views::chunk(3) |
                                                   std::views::transform(toArray<3, int>));
        if (coords.size() != 3)
        {
            throw std::invalid_argument("Expected 9 coordinates: " + std::string(line));
        }
#else
        auto numbers = readNumbers(line);
        if (numbers.size() != 9)
        {
            throw std::invalid_argument("Expected 9 coordinates: " + std::string(line));
        }
        auto coords = std::vector<std::array<int, 3>>(3);
        for (int i = 0; i < 3; ++i)
        {
            std::ranges::copy(std::next(numbers.begin(), 3 * i),
                              std::next(numbers.begin(), 3 * (i + 1)), coords[i].begin());
        }

#endif
        return Particle{coords[0], coords[1], coords[2]};
    };
    auto particles =
        std::ranges::to<std::vector>(readAllLines(input) | std::views::transform(parse));

    // asymptotic behavior is 1/2at^2, so smallest a wins.  but we also need to check v and p to
    // break ties
    auto asymptoticClosest = std::ranges::min_element(
        particles, {},
        [](auto p) { return std::make_tuple(manhattan(p.a), manhattan(p.v), manhattan(p.p)); });

    int part1 = static_cast<int>(std::distance(particles.begin(), asymptoticClosest));
    return {part1, part2(particles)};
}
} // namespace aoc
