#include "aoc.hh"
#include <algorithm>
#include <cassert>
#include <ctre.hpp>
#include <map>
#include <ranges>
#include <stdexcept>

/* https://adventofcode.com/2018/day/4
 */
namespace aoc
{
constexpr size_t YEAR = 2018;
constexpr size_t DAY = 4;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
struct Event
{
    enum class Type
    {
        ON_DUTY,
        SLEEP,
        WAKE
    };
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int guardId{};
    Type type{};
};

auto parse(std::string_view event)
{
    static constexpr auto PAT = ctll::fixed_string{R"(\[(\d+)-(\d+)-(\d+) (\d+):(\d+)\] (.*))"};
    static constexpr auto ON_DUTY_PAT = ctll::fixed_string{R"(Guard #(\d+) begins shift)"};
    enum PAT_GROUPS
    {
        FULLTEXT,
        YEAR,
        MONTH,
        DAY,
        HOUR,
        MINUTE,
        TEXT,
    };
    if (auto m = ctre::match<PAT>(event))
    {
        auto res = Event{.year = m.get<YEAR>().to_number(),
                         .month = m.get<MONTH>().to_number(),
                         .day = m.get<DAY>().to_number(),
                         .hour = m.get<HOUR>().to_number(),
                         .minute = m.get<MINUTE>().to_number()};
        auto text = m.get<TEXT>().view();
        if (text == "falls asleep")
        {
            res.type = Event::Type::SLEEP;
        }
        else if (text == "wakes up")
        {
            res.type = Event::Type::WAKE;
        }
        else if (auto mt = ctre::match<ON_DUTY_PAT>(text))
        {
            res.type = Event::Type::ON_DUTY;
            res.guardId = mt.get<1>().to_number();
        }
        else
        {
            throw std::invalid_argument(std::format("Unrecognized event text: {}", event));
        }
        return res;
    }
    throw std::invalid_argument(std::format("Unrecognized event: {}", event));
}

auto findSleepy(std::span<Event> events)
{
    constexpr static auto MINUTES = 60;
    if (events.empty())
    {
        throw std::invalid_argument("Empty event list");
    }
    if (events.front().type != Event::Type::ON_DUTY)
    {
        throw std::invalid_argument("First event not on-duty");
    }
    auto naptimes = std::map<int, std::array<int, MINUTES>>{};
    auto guard = events.front().guardId;
    auto sleep = std::optional<int>{};

    for (const auto& e : events)
    {
        if (sleep && e.type != Event::Type::WAKE)
        {
            throw std::invalid_argument(std::format("Guard {} didn't wake on {}-{}-{} {}:{}", guard,
                                                    e.year, e.month, e.day, e.hour, e.minute));
        }
        if (e.type != Event::Type::ON_DUTY && e.hour != 0)
        {
            throw std::invalid_argument("sleep/wake event outside of midnight hour");
        }
        // TODO: Check that wakeup is on same day as sleep start
        switch (e.type)
        {
        case Event::Type::ON_DUTY:
            guard = e.guardId;
            break;
        case Event::Type::SLEEP:
            sleep = e.minute;
            break;
        case Event::Type::WAKE:
            for (auto h : std::views::iota(*sleep, e.minute))
            {
                ++naptimes[guard][h];
            }
            sleep = {};
            break;
        }
    }

    const auto& p1 = *std::ranges::max_element(
        naptimes, {}, [](auto p) { return std::ranges::fold_left(p.second, 0, std::plus<>()); });
    auto p1Minute = static_cast<int>(
        std::ranges::distance(p1.second.begin(), std::ranges::max_element(p1.second)));

    const auto& p2 =
        *std::ranges::max_element(naptimes, {}, [](auto p) { return std::ranges::max(p.second); });
    auto p2Minute = static_cast<int>(
        std::ranges::distance(p2.second.begin(), std::ranges::max_element(p2.second)));

    return Solution{p1Minute * p1.first, p2Minute * p2.first};
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto lines = readAllLines(input);
    std::ranges::sort(lines);
    auto events = std::ranges::to<std::vector>(lines | std::views::transform(parse));

    return {findSleepy(events)};
}
} // namespace aoc
