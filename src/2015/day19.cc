#include "aoc.hh"
#include <map>
#include <ranges>
#include <scn/scan.h>
#include <set>
#include <string_view>

/* https://adventofcode.com/2015/day/19
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 19;

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto lines = readAllLines(input);
    std::string_view target{lines.back()};
    auto toTrans = [](std::string s)
    {
        auto result = scn::scan<std::string, std::string>(s, "{} => {}");
        return result->values();
    };
    auto trans = lines | std::views::take_while([](const auto& s) { return !s.empty(); }) |
                 std::views::transform(toTrans) |
                 std::ranges::to<std::multimap<std::string, std::string, std::less<>>>();
    std::set<std::string> seen;

    for (size_t i{}; i < target.size(); ++i)
    {
        auto search = std::string_view{&target[i], 1};
        auto productions = trans.equal_range(search);
        for (auto it = productions.first; it != productions.second; ++it)
        {
            std::string produced;
            produced.append(target.substr(0, i));
            produced.append(it->second);
            produced.append(target.substr(i + 1));
            seen.insert(std::move(produced));
        }
        if (i + 1 < target.size())
        {
            auto search = std::string_view{&target[i], 2};
            auto productions = trans.equal_range(search);
            for (auto it = productions.first; it != productions.second; ++it)
            {
                std::string produced;
                produced.append(target.substr(0, i));
                ;
                produced.append(it->second);
                produced.append(target.substr(i + 2));
                seen.insert(std::move(produced));
            }
        }
    }

    return {seen.size()};
}
} // namespace aoc
