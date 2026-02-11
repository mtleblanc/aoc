#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "aoc.hh"
namespace aoc
{
constexpr size_t YEAR = 2025;
constexpr size_t DAY = 5;
namespace
{
struct Range
{
    long min{};
    long max{};
    [[nodiscard]] bool in(long v) const
    {
        return v >= min && v <= max;
    }
    Range(const std::string& s)
    {
        char c = 0;
        std::istringstream ss{s};
        ss >> min >> c >> max;
    }
};

struct Event
{
    long id;
    long effect;
    Event(long id, long effect) : id{id}, effect{effect} {}
};
} // namespace

template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    auto lines = readAllLines(input);
    std::vector<Range> v;
    auto blank = std::ranges::find(lines, "");
    std::copy(lines.begin(), blank, std::back_inserter(v));
    long test{};
    ssize_t accum{0};
    for (; blank != lines.end(); blank++)
    {
        std::istringstream iss{*blank};
        iss >> test;
        for (auto& r : v)
        {
            if (r.in(test))
            {
                accum++;
                break;
            }
        }
    }
    std::vector<Event> es;
    es.reserve(v.size() * 2);
    for (auto& r : v)
    {
        es.emplace_back(r.min, 1);
        es.emplace_back(r.max, -1);
    }
    // comparison on effect is required to not double count endpoints
    // eg 1-10, 10-20 is a range of 20 but subranges of 10 and 11 with
    // 10 counted twice.  Without the effect sort, if the first range
    // closes before the second opens, we get the wrong answer.
    // It's also required for size-1 ranges, If we also close the a range
    // at the same location, then close the size 1 range we end up at -1
    // on, and when we open the size 1 range on goes back to 0 but start
    // never got updated.  This could of course be checked for, but
    // we need the sorting for the first effect anyway
    std::sort(es.begin(), es.end(), [](const Event& a, const Event& b)
              { return a.id < b.id || (a.id == b.id && a.effect > b.effect); });
    ssize_t accum2{0};
    long start{};
    long on{0};
    for (auto& e : es)
    {
        if (on == 0 && e.effect == 1)
        {
            start = e.id;
        }
        on += e.effect;
        if (on == 0)
        {
            accum2 += e.id - start + 1;
        }
    }

    return SsizeSolution{accum, accum2};
}

} // namespace aoc