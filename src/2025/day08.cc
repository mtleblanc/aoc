#include <cassert>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

#include "aoc.hh"
namespace aoc
{
constexpr size_t YEAR = 2025;
constexpr size_t DAY = 8;
namespace
{
struct Box
{
    ssize_t x;
    ssize_t y;
    ssize_t z;
    ssize_t sz{1};
    Box* parent{nullptr};
    [[nodiscard]] int64_t distanceSquare(const Box& other) const
    {
        return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y) +
               (z - other.z) * (z - other.z);
    }
    Box(int64_t x, int64_t y, int64_t z) : x{x}, y{y}, z{z} {}

    Box* rep()
    {
        if (parent == nullptr)
        {
            return this;
        }
        parent = parent->rep();
        return parent;
    }

    void join(Box& o)
    {
        Box* p1 = rep();
        Box* p2 = o.rep();
        if (p1 == p2)
        {
            return;
        }
        ssize_t newSz = p1->sz + p2->sz;
        o.rep()->parent = rep();
        rep()->sz = newSz;
    }
};

// std::ostream& operator<<(std::ostream& os, const Box& b)
// {
//     return os << b.x << "," << b.y << "," << b.z;
// }
struct Wire
{
    std::shared_ptr<Box> b1;
    std::shared_ptr<Box> b2;
    int64_t distanceSquare{};
    Wire() : b1{nullptr}, b2{nullptr} {}
    Wire(const std::shared_ptr<Box>& b1, const std::shared_ptr<Box>& b2)
        : b1{b1}, b2{b2}, distanceSquare(b1->distanceSquare(*b2))
    {
    }
    bool operator<(const Wire& o) const
    {
        return distanceSquare > o.distanceSquare;
    }
};

// std::ostream& operator<<(std::ostream& os, const wire& b)
// {
//     return os << *(b.b1) << " to " << *(b.b2);
// }

template <typename T> void print(std::vector<T>& v)
{
    for (auto i : v)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

ssize_t part1(const std::vector<std::string>& v, size_t connections)
{
    std::vector<std::shared_ptr<Box>> boxes{};
    std::priority_queue<Wire> wires{};
    for (const auto& in : v)
    {
        ssize_t x = 0;
        ssize_t y = 0;
        ssize_t z = 0;
        char c = 0;
        std::istringstream ss{in};
        ss >> x >> c >> y >> c >> z;
        auto b = std::make_shared<Box>(x, y, z);
        for (auto& o : boxes)
        {
            wires.emplace(o, b);
        }
        boxes.push_back(b);
    }
    for (size_t i = 0; i < connections; i++)
    {
        Wire w;

        w = wires.top();
        wires.pop();
        // std::cout << "Considering " << *(w.b1) << " to " << *(w.b2) << std::endl;

        // } while (w.b1->rep() == w.b2->rep());
        // std::cout << "Joined" << std::endl;
        w.b1->join(*w.b2);
    }
    ssize_t accum{1};
    std::priority_queue<ssize_t> sizes{};
    for (auto& b : boxes)
    {
        if (b->parent == nullptr)
        {
            sizes.push(b->sz);
        }
    }
    for (size_t i = 0; i < 3; i++)
    {
        accum *= sizes.top();
        sizes.pop();
    }
    return accum;
}

ssize_t part2(const std::vector<std::string>& v)
{
    std::vector<std::shared_ptr<Box>> boxes{};
    std::priority_queue<Wire> wires{};
    for (const auto& in : v)
    {
        ssize_t x = 0;
        ssize_t y = 0;
        ssize_t z = 0;
        char c = 0;
        std::istringstream ss{in};
        ss >> x >> c >> y >> c >> z;
        auto b = std::make_shared<Box>(x, y, z);
        for (auto& o : boxes)
        {
            wires.emplace(o, b);
        }
        boxes.push_back(b);
    }
    Wire w;
    while (boxes[0]->rep()->sz < std::ssize(boxes))
    {
        w = wires.top();
        wires.pop();
        // std::cout << "Considering " << *(w.b1) << " to " << *(w.b2) << std::endl;

        // } while (w.b1->rep() == w.b2->rep());
        // std::cout << "Joined" << std::endl;
        w.b1->join(*w.b2);
    }
    return w.b1->x * w.b2->x;
}

} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto lines = readAllLines(input);
    constexpr size_t SAMPLE_CONNECTIONS = 10;
    constexpr size_t ACTUAL_CONNECTIONS = 1000;
    return Solution{
        part1(lines, lines.size() < ACTUAL_CONNECTIONS ? SAMPLE_CONNECTIONS : ACTUAL_CONNECTIONS),
        part2(lines)};
}
} // namespace aoc