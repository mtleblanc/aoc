#include "aoc.hh"
#include "util.hh"
#include <algorithm>
#include <cmath>
#include <exception>
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

struct Particle
{
    vec p;
    vec v;
    vec a;
    bool alive{true};

    Particle(vec p, vec v, vec a) : p{p}, v{v}, a{a} {}
    void update()
    {
        v += a;
        p += v;
    }

    Particle operator-(const Particle& o) const
    {
        return Particle{p - o.p, v - o.v, a - o.a};
    }

    [[nodiscard]] std::optional<int> firstZero() const
    {
        // 0 = p + vt + 1/2a t^2
        // t = (-v +/- sqrt(v^2 - 2ap))/2a
        // TODO: is this helpful
        if ((std::signbit(p[0]) == std::signbit(v[0]) &&
             std::signbit(p[0]) == std::signbit(a[0])) ||
            (std::signbit(p[0]) == std::signbit(v[0]) &&
             std::signbit(p[0]) == std::signbit(a[0])) ||
            (std::signbit(p[0]) == std::signbit(v[0]) && std::signbit(p[0]) == std::signbit(a[0])))
        {
            return {};
        }
        if (a[0] == 0)
        {
            if (v[0] == 0)
            {
                return {};
            }
            auto t = -p[0] / v[0];
            if (t >= 0 && t * v[0] == -p[0] && 2 * p[1] + 2 * v[1] * t + a[1] * t * t == 0 &&
                2 * p[2] + 2 * v[2] * t + a[2] * t * t == 0)
            {
                return t;
            }
            return {};
        }
        auto sq = v[0] * v[0] - 2 * a[0] * p[0];
        // TODO: is this helpful
        if (sq < 0 || sq % 4 == 2 || sq % 4 == 3)
        {
            return {};
        }
        auto sqrt = static_cast<int>(std::sqrt(sq));
        if (sqrt * sqrt != sq)
        {
            return {};
        }
        std::vector<int> ts;
        auto numerator = -v[0] + sqrt;
        auto denominator = 2 * a[0];
        ts.push_back((numerator % denominator == 0) ? numerator / denominator : -1);
        numerator = -v[0] - sqrt;
        ts.push_back((numerator % denominator == 0) ? numerator / denominator : -1);
        std::vector<int> valid{};
        for (auto t : ts)
        {
            if (t >= 0)
            {
                if (2 * p[1] + 2 * v[1] * t + a[1] * t * t == 0 &&
                    2 * p[2] + 2 * v[2] * t + a[2] * t * t == 0)
                {
                    valid.push_back(t);
                }
            }
        }
        if (valid.empty())
        {
            return {};
        }
        return std::ranges::min(valid);
    }

    auto operator<=>(const Particle& o) const = default;
};

struct Collision
{
    int time;
    std::pair<int, int> indices;
    Collision(int time, std::pair<int, int> indices) : time{time}, indices{indices} {}
    auto operator<=>(const Collision& o) const = default;
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
        auto time = collisions.top().time;
        auto toCollide = std::vector<int>{};
        while (collisions.size() && collisions.top().time == time)
        {
            auto [l, r] = collisions.top().indices;
            if (particles[l].alive && particles[r].alive)
            {
                toCollide.push_back(l);
                toCollide.push_back(r);
            }
            collisions.pop();
        }
        for (auto i : toCollide)
        {
            particles[i].alive = false;
        }
    }

    return static_cast<int>(std::ranges::count_if(particles, [](auto p) { return p.alive; }));
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto parse = [](auto line)
    {
        auto coords = readNumbers(line) | std::views::chunk(3) |
                      std::views::transform(toArray<3, int>) | std::ranges::to<std::vector>();
        if (coords.size() != 3)
        {
            throw std::invalid_argument("Expected 9 coordinates: " + std::string(line));
        }
        return Particle{coords[0], coords[1], coords[2]};
    };
    auto particles =
        readAllLines(input) | std::views::transform(parse) | std::ranges::to<std::vector>();

    auto fastest = std::ranges::min_element(
        particles, {},
        [](auto p) { return std::make_tuple(manhattan(p.a), manhattan(p.v), manhattan(p.p)); });

    int part1 = static_cast<int>(std::distance(particles.begin(), fastest));
    // for (int step{0}; step < 1000000; ++step)
    // {
    //     std::ranges::sort(particles);
    //     for (int i{}; i + 1 < std::ssize(particles); ++i)
    //     {
    //         if (particles[i].p == particles[i + 1].p)
    //         {
    //             auto collision = particles[i].p;
    //             while (particles[i].p == collision)
    //             {
    //                 particles.erase(std::next(particles.begin(), i));
    //             }
    //         }
    //     }
    //     for (auto& p : particles)
    //     {
    //         p.update();
    //     }
    // }
    return {part1, part2(particles)};
}
} // namespace aoc
