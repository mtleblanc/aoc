#include "aoc.hh"
#include <algorithm>
#include <ctre.hpp>
#include <functional>
#include <map>
#include <optional>
#include <ranges>

/* https://adventofcode.com/2016/day/10
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 10;

namespace
{
struct Bot;
struct Output;

struct Dest
{
    using ReportCB = std::function<void(int, int, int)>;
    virtual ~Dest() = default;
    Dest(const Dest&) = default;
    Dest& operator=(const Dest&) = default;
    Dest(Dest&&) = default;
    Dest& operator=(Dest&&) = default;
    virtual void accept(int v, [[maybe_unused]] const ReportCB& report) = 0;

  protected:
    Dest() = default;
};
struct Bot : public Dest
{
    int id{};
    std::optional<int> value;
    Dest* low{};
    Dest* high{};

    Bot() = default;

    void accept(int v, [[maybe_unused]] const ReportCB& report) override
    {
        if (!value)
        {
            value = v;
        }
        else
        {
            auto [l, h] = std::minmax(*value, v);
            if (report)
            {
                report(id, l, h);
            }
            value.reset();
            low->accept(l, report);
            high->accept(h, report);
        }
    }
};

struct Output : public Dest
{
    std::vector<int> values;

    Output() = default;

    void accept(int v, [[maybe_unused]] const ReportCB& report) override
    {
        values.push_back(v);
    }
};

struct Input
{
    int value;
    Dest* dest;
};

Solution_t<YEAR, DAY> simulate(const std::vector<std::string>& descs)
{
    int part1{};
    auto report = [&part1](int bot, int low, int high)
    {
        constexpr int LOW = 17;
        constexpr int HIGH = 61;
        if (low == LOW && high == HIGH)
        {
            part1 = bot;
        }
    };
    std::map<int, Bot> bots;
    std::map<int, Output> outputs;
    std::vector<Input> inputs;
    for (const auto& desc : descs)
    {
        constexpr auto BOT_PAT = ctll::fixed_string(
            R"(bot (\d+) gives low to (bot|output) (\d+) and high to (bot|output) (\d+))");
        constexpr size_t BOT_ID = 1;
        constexpr size_t BOT_LOW_IS_BOT = 2;
        constexpr size_t BOT_LOW_ID = 3;
        constexpr size_t BOT_HIGH_IS_BOT = 4;
        constexpr size_t BOT_HIGH_ID = 5;
        constexpr auto INPUT_PAT = ctll::fixed_string(R"(value (\d+) goes to bot (\d+))");
        constexpr auto INPUT_VAL = 1;
        constexpr auto INPUT_ID = 2;
        if (auto m = ctre::match<BOT_PAT>(desc))
        {
            auto id = toNum<int>(m.get<BOT_ID>());
            auto& bot = bots[id];
            bot.id = id;

            auto isBot = std::string(m.get<BOT_LOW_IS_BOT>()) == "bot";
            auto destId = toNum<int>(m.get<BOT_LOW_ID>());
            bot.low =
                isBot ? static_cast<Dest*>(&bots[destId]) : static_cast<Dest*>(&outputs[destId]);
            isBot = std::string(m.get<BOT_HIGH_IS_BOT>()) == "bot";
            destId = toNum<int>(m.get<BOT_HIGH_ID>());
            bot.high =
                isBot ? static_cast<Dest*>(&bots[destId]) : static_cast<Dest*>(&outputs[destId]);
        }
        else if (auto m = ctre::match<INPUT_PAT>(desc))
        {
            auto val = toNum<int>(m.get<INPUT_VAL>());
            auto id = toNum<int>(m.get<INPUT_ID>());
            inputs.emplace_back(val, &bots[id]);
        }
        else
        {
            throw std::invalid_argument("Bad bot description");
        }
    }
    for (const auto& input : inputs)
    {
        input.dest->accept(input.value, report);
    }

    auto firstThreeOutputs = std::views::iota(0UL, 3UL) |
                             std::views::transform([&outputs](auto i) { return outputs[i]; });
    if (std::ranges::any_of(firstThreeOutputs, [](const auto& o) { return o.values.size() != 1; }))
    {
        throw std::runtime_error("Outputs 0-2 did not each have a single element");
    }
    auto part2 = std::ranges::fold_left(
        firstThreeOutputs | std::views::transform([](auto o) { return o.values[0]; }), 1,
        std::multiplies<>());
    return {part1, part2};
}
} // namespace

template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    auto descs = readAllLines(input);
    return simulate(descs);
}
} // namespace aoc
