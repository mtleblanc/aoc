#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct box
{
    int64_t x;
    int64_t y;
    int64_t z;
    size_t sz;
    box *parent{nullptr};
    int64_t distanceSquare(const box &other)
    {
        return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y) + (z - other.z) * (z - other.z);
    }
    box(int64_t x, int64_t y, int64_t z) : x{x}, y{y}, z{z}, sz{1}
    {
    }

    box *rep()
    {
        if (parent == nullptr)
            return this;
        parent = parent->rep();
        return parent;
    }

    void join(box &o)
    {
        box *p1 = rep();
        box *p2 = o.rep();
        if (p1 == p2)
            return;
        size_t new_sz = p1->sz + p2->sz;
        o.rep()->parent = rep();
        rep()->sz = new_sz;
    }
};

std::ostream &operator<<(std::ostream &os, const box &b)
{
    return os << b.x << "," << b.y << "," << b.z;
}
struct wire
{
    std::shared_ptr<box> b1;
    std::shared_ptr<box> b2;
    int64_t distanceSquare;
    wire() : b1{nullptr}, b2{nullptr} {}
    wire(std::shared_ptr<box> b1, std::shared_ptr<box> b2) : b1{b1}, b2{b2}
    {
        distanceSquare = b1->distanceSquare(*b2.get());
    }
    bool operator<(const wire &o) const
    {
        return distanceSquare > o.distanceSquare;
    }
};
std::ostream &operator<<(std::ostream &os, const wire &b)
{
    return os << *(b.b1) << " to " << *(b.b2);
}

template <typename T>
void print(std::vector<T> &v)
{
    for (auto i : v)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

uint64_t
part1(std::vector<std::string> &v, size_t connections)
{
    std::vector<std::shared_ptr<box>> boxes{};
    std::priority_queue<wire> wires{};
    for (auto &in : v)
    {
        int64_t x;
        int64_t y;
        int64_t z;
        char c;
        std::istringstream ss{in};
        ss >> x >> c >> y >> c >> z;
        auto b = std::make_shared<box>(x, y, z);
        for (auto &o : boxes)
        {
            wires.emplace(o, b);
        }
        boxes.push_back(b);
    }
    for (size_t i = 0; i < connections; i++)
    {
        wire w;
        do
        {
            w = wires.top();
            wires.pop();
            // std::cout << "Considering " << *(w.b1) << " to " << *(w.b2) << std::endl;
        } while (0);
        // } while (w.b1->rep() == w.b2->rep());
        // std::cout << "Joined" << std::endl;
        w.b1->join(*w.b2);
    }
    uint64_t accum{1};
    std::priority_queue<uint64_t> sizes{};
    for (auto &b : boxes)
    {
        if (b->parent == nullptr)
            sizes.push(b->sz);
    }
    for (size_t i = 0; i < 3; i++)
    {
        accum *= sizes.top();
        sizes.pop();
    }
    return accum;
}

uint64_t part2(std::vector<std::string> &v)
{
    std::vector<std::shared_ptr<box>> boxes{};
    std::priority_queue<wire> wires{};
    for (auto &in : v)
    {
        int64_t x;
        int64_t y;
        int64_t z;
        char c;
        std::istringstream ss{in};
        ss >> x >> c >> y >> c >> z;
        auto b = std::make_shared<box>(x, y, z);
        for (auto &o : boxes)
        {
            wires.emplace(o, b);
        }
        boxes.push_back(b);
    }
    wire w;
    while (boxes[0]->rep()->sz < boxes.size())
    {
        do
        {
            w = wires.top();
            wires.pop();
            // std::cout << "Considering " << *(w.b1) << " to " << *(w.b2) << std::endl;
        } while (0);
        // } while (w.b1->rep() == w.b2->rep());
        // std::cout << "Joined" << std::endl;
        w.b1->join(*w.b2);
    }
    return w.b1->x * w.b2->x;
}

int main()
{
    std::ifstream fs{"input08.txt"};
    std::vector<std::string> v;
    for (std::string s; getline(fs, s);)
    {
        if (s.length() == 0)
            break;
        v.emplace_back(s);
    }

    std::cout << part1(v, 1000) << std::endl;
    std::cout << part2(v) << std::endl;
}