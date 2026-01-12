#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include "aoc.hh"
namespace aoc
{
constexpr size_t YEAR = 2025;
constexpr size_t DAY = 11;
namespace
{
struct Node
{
    std::string id;
    std::vector<std::string> outputs;
    std::optional<uint64_t> paths;
    std::optional<uint64_t> pathsNoDac;
    std::optional<uint64_t> pathsNoFft;
    std::optional<uint64_t> pathsNeither;
};

std::istream& operator>>(std::istream& is, Node& n)
{
    is >> n.id;
    if (!n.id.ends_with(':'))
    {
        throw std::invalid_argument("Malformed input");
    }
    n.id.pop_back();
    std::string tmp;
    while (is >> tmp)
    {
        n.outputs.push_back(std::move(tmp));
    }

    return is;
}

void countPaths(const std::string& source, auto& m)
{
    Node& n = m[source];
    if (n.paths)
    {
        return;
    }
    std::ranges::for_each(n.outputs, [&m](const auto& id) { countPaths(id, m); });

    n.paths = std::accumulate(n.outputs.begin(), n.outputs.end(), 0,
                              [&m](auto v, const auto& id) { return v + m[id].paths.value(); });

    // if (!n.pathsNoDac.has_value())
    // {
    //     n.pathsNoDac =
    //         std::accumulate(n.outputs.begin(), n.outputs.end(), 0,
    //                         [&m](auto v, const auto& id) { return v + m[id].pathsNoDac.value();
    //                         });
    // }

    // if (!n.pathsNoFft.has_value())
    // {
    //     n.pathsNoFft =
    //         std::accumulate(n.outputs.begin(), n.outputs.end(), 0,
    //                         [&m](auto v, const auto& id) { return v + m[id].pathsNoFft.value();
    //                         });
    // }

    // if (!n.pathsNeither.has_value())
    // {
    //     n.pathsNeither =
    //         std::accumulate(n.outputs.begin(), n.outputs.end(), 0,
    //                         [&m](auto v, const auto& id) { return v + m[id].pathsNeither.value();
    //                         });
    // }
}

size_t part1(std::vector<Node>& ns)
{
    std::map<std::string, Node> m;
    for (const auto& n : ns)
    {
        m[n.id] = n;
    }
    m["out"].paths = 1;
    countPaths(std::string{"you"}, m);
    return m["you"].paths.value();
}

size_t part2(std::vector<Node>& ns)
{
    std::map<std::string, Node> m;
    for (const auto& n : ns)
    {
        m[n.id] = n;
    }

    auto mt = m;
    mt["fft"].paths = 1;
    mt["out"].paths = 0;
    countPaths("svr", mt);
    size_t svrToDac = mt["svr"].paths.value();
    mt = m;
    mt["dac"].paths = 1;
    mt["out"].paths = 0;
    countPaths("fft", mt);
    size_t dacToFft = mt["fft"].paths.value();
    mt = m;

    mt["out"].paths = 1;
    countPaths("dac", mt);
    size_t fftToOut = mt["dac"].paths.value();
    return svrToDac * dacToFft * fftToOut;

    // m["out"].paths = 1;
    // m["out"].pathsNoDac = 1;
    // m["out"].pathsNoFft = 1;
    // m["out"].pathsNeither = 1;
    // m["dac"].pathsNeither = 0;
    // m["dac"].pathsNoDac = 0;
    // m["fft"].pathsNeither = 0;
    // m["fft"].pathsNoFft = 0;
    // countPaths("svr", m);
    // auto& n = m["svr"];
    // return n.paths.value() - n.pathsNoFft.value() - n.pathsNoDac.value() +
    // n.pathsNeither.value();
}
} // namespace
template <> Solution solve<YEAR, DAY>(const std::vector<std::string>& lines)
{
    std::vector<Node> v;
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
    return Solution{part1(v), part2(v)};
}
} // namespace aoc