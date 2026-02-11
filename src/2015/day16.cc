#include "aoc.hh"
#include <algorithm>
#include <map>
#include <sstream>

/* https://adventofcode.com/2015/day/16
 */
namespace aoc
{
constexpr ssize_t YEAR = 2015;
constexpr size_t DAY = 16;

namespace
{
struct Sue
{
    ssize_t id{};
    std::map<std::string, ssize_t> owns;
};

std::istream& operator>>(std::istream& is, Sue& s)
{
    // Sue 1: children: 1, cars: 8, vizslas: 7
    std::string line;
    if (!std::getline(is, line))
    {
        return is;
    }

    std::istringstream iss{line};
    std::string label;
    char colon{};
    iss >> label >> s.id >> colon;
    if (label != "Sue" || colon != ':')
    {
        is.setstate(std::ios_base::failbit);
        return is;
    }

    s.owns.clear();
    for (std::string item; std::getline(iss, item, ',');)
    {
        std::istringstream itemStream{item};
        std::string name;
        ssize_t amount{};
        itemStream >> name >> amount;
        if (name.empty() || name.back() != ':')
        {
            is.setstate(std::ios_base::failbit);
            return is;
        }
        name.pop_back();
        s.owns[name] = amount;
    }
    return is;
}

class Matcher
{
  public:
    enum class Op : uint8_t
    {
        EQ,
        LT,
        GT
    };
    Matcher(ssize_t compareTo, Op op) : compareTo{compareTo}, op{op} {}
    [[nodiscard]] bool operator()(ssize_t o) const
    {
        switch (op)
        {
        case Op::EQ:
            return o == compareTo;
        case Op::LT:
            return o < compareTo;
        case Op::GT:
            return o > compareTo;
        }
    }

    [[nodiscard]] bool eq(ssize_t o) const
    {
        return o == compareTo;
    }

  private:
    ssize_t compareTo;
    Op op;
};

Matcher eq(ssize_t value)
{
    return Matcher{value, Matcher::Op::EQ};
}
Matcher lt(ssize_t value)
{
    return Matcher{value, Matcher::Op::LT};
}
Matcher gt(ssize_t value)
{
    return Matcher{value, Matcher::Op::GT};
}
} // namespace

template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    // NOLINTBEGIN
    std::map<std::string, Matcher> search{{"children", eq(3UL)}, {"cats", gt(7UL)},
                                          {"samoyeds", eq(2UL)}, {"pomeranians", lt(3UL)},
                                          {"akitas", eq(0UL)},   {"vizslas", eq(0UL)},
                                          {"goldfish", lt(5UL)}, {"trees", gt(3UL)},
                                          {"cars", eq(2UL)},     {"perfumes", eq(1UL)}};
    // NOLINTEND

    std::vector<Sue> sues;
    for (Sue s; input >> s;)
    {
        sues.push_back(s);
    }

    using namespace std::ranges;
    auto part1matches = [&search](auto& sue)
    {
        return all_of(search,
                      [&sue](auto& item)
                      {
                          auto& [id, pred] = item;
                          return !sue.owns.contains(id) || pred.eq(sue.owns[id]);
                      });
    };

    auto part2matches = [&search](auto& sue)
    {
        return all_of(search,
                      [&sue](auto& item)
                      {
                          auto& [id, pred] = item;
                          return !sue.owns.contains(id) || pred(sue.owns[id]);
                      });
    };

    return {find_if(sues, part1matches)->id, find_if(sues, part2matches)->id};
}
} // namespace aoc
