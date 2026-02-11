#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "aoc.hh"
namespace aoc
{
constexpr size_t YEAR = 2025;
constexpr size_t DAY = 9;
namespace
{
struct Corner
{
    ssize_t x;
    ssize_t y;
};

ssize_t area(const Corner& c1, const Corner& c2)
{
    return (std::abs(c1.x - c2.x) + 1) * (std::abs(c1.y - c2.y) + 1);
}

// std::ostream& operator<<(std::ostream& os, const Corner& b)
// {
//     return os << b.x << "," << b.y;
// }

std::istream& operator>>(std::istream& is, Corner& corner)
{
    char c{};
    return is >> corner.x >> c >> corner.y;
}

ssize_t part1(const std::vector<Corner>& v)
{
    ssize_t accum{};
    for (size_t i = 0; i < v.size(); i++)
    {
        for (size_t j = 0; j < i; j++)
        {
            accum = std::max(accum, area(v[i], v[j]));
        }
    }
    return accum;
}

bool intersects(const Corner c1, const Corner c2, const Corner e1, const Corner e2)
{
    if (e1.x == e2.x)
    {
        if (e1.x >= std::max(c1.x, c2.x) || e1.x <= std::min(c1.x, c2.x))
        {
            return false;
        }
        if (std::min(e1.y, e2.y) >= std::max(c1.y, c2.y) ||
            std::max(e1.y, e2.y) <= std::min(c1.y, c2.y))
        {
            return false;
        }
        return true;
    }

    if (e1.y >= std::max(c1.y, c2.y) || e1.y <= std::min(c1.y, c2.y))
    {
        return false;
    }
    if (std::min(e1.x, e2.x) >= std::max(c1.x, c2.x) ||
        std::max(e1.x, e2.x) <= std::min(c1.x, c2.x))
    {
        return false;
    }
    return true;
}

ssize_t part2(const std::vector<Corner>& v)
{
    ssize_t accum{};
    for (size_t i = 0; i < v.size(); i++)
    {
        for (size_t j = 0; j < i; j++)
        {
            const auto a = area(v[i], v[j]);
            if (a <= accum)
            {
                continue;
            }
            for (size_t k = 0; k < v.size(); k++)
            {
                if (intersects(v[i], v[j], v[k], v[(k + 1) % v.size()]))
                {
                    goto cont;
                }
            }
            accum = a;
        cont:
            continue;
        }
    }
    return accum;
}

} // namespace

template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    auto lines = readAllLines(input);
    std::vector<Corner> v;
    for (const auto& s : lines)
    {
        if (s.length() == 0)
        {
            break;
        }
        std::istringstream iss{s};
        v.emplace_back();
        iss >> v.back();
    }
    return SsizeSolution{part1(v), part2(v)};
}
} // namespace aoc