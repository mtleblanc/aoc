#include "aoc.hh"
#include <algorithm>
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

namespace
{
auto part1(const std::multimap<std::string, std::string, std::less<>>& productions,
           std::string_view target)
{
    std::set<std::string> seen;

    for (ssize_t i{}; i < std::ssize(target); ++i)
    {
        auto search = std::string_view{&target[i], 1};
        auto ps = productions.equal_range(search);
        for (auto it = ps.first; it != ps.second; ++it)
        {
            std::string produced;
            produced.append(target.substr(0, i));
            produced.append(it->second);
            produced.append(target.substr(i + 1));
            seen.insert(std::move(produced));
        }
        if (i + 1 < std::ssize(target))
        {
            auto search = std::string_view{&target[i], 2};
            auto ps = productions.equal_range(search);
            for (auto it = ps.first; it != ps.second; ++it)
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
    return std::ssize(seen);
}

struct EarleyState
{
    using Production = const std::pair<const std::string, std::string>;
    ssize_t origin{};
    Production* production;
    ssize_t progress{};
    ssize_t depth{1};

    auto operator<=>(const EarleyState& o) const
    {
        return std::tie(origin, production, progress) <=>
               std::tie(o.origin, o.production, o.progress);
    }

    bool operator==(const EarleyState& o) const
    {
        return (*this <=> o) == std::strong_ordering::equal;
    };

    [[nodiscard]] auto isComplete() const
    {
        return progress == std::ssize(production->second);
    }
    [[nodiscard]] std::string next() const
    {
        return production->second.substr(progress, 1);
    }

    EarleyState(Production& production, ssize_t origin = 0, ssize_t progress = 0, ssize_t depth = 1)
        : origin{origin}, production{&production}, progress{progress}, depth{depth}
    {
    }

    EarleyState(Production* production, ssize_t origin = 0, ssize_t progress = 0, ssize_t depth = 1)
        : origin{origin}, production{production}, progress{progress}, depth{depth}
    {
    }

    [[nodiscard]] EarleyState scan(ssize_t d = 0) const
    {
        (void)d;
        return EarleyState{production, origin, progress + 1, depth + d};
    }

    [[maybe_unused]] friend std::ostream& operator<<(std::ostream& os, const EarleyState& es)
    {
        std::string_view rhs = es.production->second;
        os << es.production->first << " -> " << rhs.substr(0, es.progress) << "*"
           << rhs.substr(es.progress) << " (" << es.origin << ") depth: " << es.depth;
        return os;
    }
};

struct EarleyParser
{
    const std::multimap<std::string, std::string, std::less<>>* productions;
    std::string_view target;
    std::vector<std::vector<EarleyState>> states;

    EarleyParser(const std::multimap<std::string, std::string, std::less<>>& productions,
                 std::string_view target)
        : productions{&productions}, target{target}, states(target.size() + 1)
    {
    }

    auto add(EarleyState state, ssize_t k)
    {
        if (std::ranges::contains(states[k], state))
        {
            return false;
        }
        states[k].push_back(state);
        // std::cout << state << std::endl;
        return true;
    };

    auto predict(EarleyState state, ssize_t k)
    {
        if (state.isComplete())
        {
            return;
        }
        auto n = state.next();
        auto predictions = productions->equal_range(n);
        for (auto it = predictions.first; it != predictions.second; ++it)
        {
            EarleyState predicted{*it, k};
            add(predicted, k);
        }
    };
    auto scan(auto state, ssize_t k)
    {
        if (state.isComplete())
        {
            return;
        }
        auto n = state.next();
        if (target[k] != n[0])
        {
            return;
        }
        auto scanned = state.scan();
        add(scanned, k + 1);
    };
    auto complete(auto state, auto k)
    {
        if (!state.isComplete())
        {
            return;
        }
        auto n = state.production->first;
        for (ssize_t i{}; i < std::ssize(states[state.origin]); ++i)
        {
            auto s = states[state.origin][i];
            if (s.isComplete())
            {
                continue;
            }
            if (s.next() != n)
            {
                continue;
            }
            auto completed = s.scan(state.depth);
            add(completed, k);
        }
    };

    bool parse()
    {
        auto initial = productions->equal_range("e");
        for (auto it = initial.first; it != initial.second; ++it)
        {
            auto state = EarleyState{*it};
            states[0].push_back(state);
        }

        for (ssize_t k{}; k < std::ssize(states); ++k)
        {
            for (ssize_t current{}; current < std::ssize(states[k]); ++current)
            {
                auto state = states[k][current];
                predict(state, k);
                if (k + 1 < std::ssize(states))
                {
                    scan(state, k);
                }
                complete(state, k);
            }
        }
        return std::ranges::any_of(states.back(), [](auto s)
                                   { return s.production->first == "e" && s.isComplete(); });
    }

    ssize_t depth()
    {
        auto parses =
            states.back() |
            std::views::filter([](auto s) { return s.production->first == "e" && s.isComplete(); });
        return parses.empty() ? 0 : std::ranges::min(parses, {}, &EarleyState::depth).depth;
    }
};

} // namespace

template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    auto lines = readAllLines(input);
    std::string_view target{lines.back()};
    auto toProduction = [](std::string s)
    {
        auto result = scn::scan<std::string, std::string>(s, "{} => {}");
        return result->values();
    };
    auto productions = lines | std::views::take_while([](const auto& s) { return !s.empty(); }) |
                       std::views::transform(toProduction) |
                       std::ranges::to<std::multimap<std::string, std::string, std::less<>>>();

    EarleyParser parser{productions, target};
    parser.parse();

    return {part1(productions, target), parser.depth()};
}
} // namespace aoc
