#include "aoc.hh"
#include <algorithm>
#include <ranges>
#include <regex>

/* https://adventofcode.com/2015/day/14
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 14;

namespace
{
struct Reindeer
{
    ssize_t speed{};
    ssize_t time{};
    ssize_t rest{};
    ssize_t current{};
    ssize_t points{};
};
std::istream& operator>>(std::istream& is, Reindeer& r)
{
    // Vixen can fly 8 km/s for 8 seconds, but then must rest for 53 seconds.
    std::regex reg{
        R"(\w+ can fly (\d+) km/s for (\d+) seconds, but then must rest for (\d+) seconds.)"};
    std::smatch match;
    std::string in;
    std::getline(is, in);
    if (!std::regex_match(in, match, reg))
    {
        is.setstate(std::ios_base::failbit);
        return is;
    }
    r.speed = stol(match[1]);
    r.time = stol(match[2]);
    r.rest = stol(match[3]);
    return is;
}

// leader's distance after RACE_DURATION seconds
template <ssize_t RACE_DURATION> auto distance(const Reindeer& r)
{
    auto lapDistance = r.speed * r.time;
    auto lapTime = r.time + r.rest;
    auto fullLaps = RACE_DURATION / lapTime;
    auto rem = RACE_DURATION % lapTime;
    auto finalLapDistance = std::min(r.time, rem) * r.speed;
    return lapDistance * fullLaps + finalLapDistance;
}

// longest time spect leading over RACE_DURATION seconds
template <ssize_t RACE_DURATION> ssize_t race(std::vector<Reindeer>& deer)
{
    auto isMoving = [](auto i)
    { return [i](const auto& d) { return i % (d.time + d.rest) < d.time; }; };
    auto move = [](auto& d) { d.current += d.speed; };
    auto isLeading = [](auto x) { return [x](const auto& d) { return d.current == x; }; };
    auto score = [](auto& d) { ++d.points; };
    for (ssize_t i = 0; i < RACE_DURATION; ++i)
    {
        std::ranges::for_each(deer | std::views::filter(isMoving(i)), move);
        auto leadDistance = std::ranges::max(deer, {}, &Reindeer::current).current;
        std::ranges::for_each(deer | std::views::filter(isLeading(leadDistance)), score);
    }
    return std::ranges::max(deer, {}, &Reindeer::points).points;
}

} // namespace
template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    constexpr ssize_t RACE_DURATION = 2503;
    std::vector<Reindeer> deer;
    for (Reindeer r; input >> r;)
    {
        deer.push_back(r);
    }
    return {std::ranges::max(deer | std::views::transform(distance<RACE_DURATION>)),
            race<RACE_DURATION>(deer)};
}
} // namespace aoc
