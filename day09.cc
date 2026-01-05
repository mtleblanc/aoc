#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct corner
{
    int64_t x;
    int64_t y;
};

uint64_t area(corner &c1, corner &c2)
{
    return (std::abs(c1.x - c2.x) + 1) * (std::abs(c1.y - c2.y) + 1);
}

std::ostream &operator<<(std::ostream &os, const corner &b)
{
    return os << b.x << "," << b.y;
}

std::istream &operator>>(std::istream &is, corner &corner)
{
    char c;
    return is >> corner.x >> c >> corner.y;
}

uint64_t
part1(std::vector<corner> &v)
{
    uint64_t accum{};
    for (size_t i = 0; i < v.size(); i++)
        for (size_t j = 0; j < i; j++)
        {
            accum = std::max(accum, area(v[i], v[j]));
        }
    return accum;
}

bool intersects(const corner c1, const corner c2, const corner e1, const corner e2)
{
    if (e1.x == e2.x)
    {
        if (e1.x >= std::max(c1.x, c2.x) || e1.x <= std::min(c1.x, c2.x))
            return false;
        if (std::min(e1.y, e2.y) >= std::max(c1.y, c2.y) || std::max(e1.y, e2.y) <= std::min(c1.y, c2.y))
            return false;
        return true;
    }

    if (e1.y >= std::max(c1.y, c2.y) || e1.y <= std::min(c1.y, c2.y))
        return false;
    if (std::min(e1.x, e2.x) >= std::max(c1.x, c2.x) || std::max(e1.x, e2.x) <= std::min(c1.x, c2.x))
        return false;
    return true;
}

uint64_t part2(std::vector<corner> &v)
{
    uint64_t accum{};
    for (size_t i = 0; i < v.size(); i++)
        for (size_t j = 0; j < i; j++)
        {
            const auto a = area(v[i], v[j]);
            if (a <= accum)
                continue;
            for (size_t k = 0; k < v.size(); k++)
            {
                if(intersects(v[i], v[j], v[k], v[(k+1)%v.size()])) goto cont;
            }
            accum = a;
            cont:
            continue;
        }
    return accum;
}

int main()
{
    std::ifstream fs{"input09.txt"};
    std::vector<corner> v;
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