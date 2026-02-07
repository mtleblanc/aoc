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
    using ReportCB = std::function<void(size_t, size_t, size_t)>;
    virtual ~Dest() = default;
    Dest(const Dest&) = default;
    Dest& operator=(const Dest&) = default;
    Dest(Dest&&) = default;
    Dest& operator=(Dest&&) = default;
    virtual void accept(size_t v, [[maybe_unused]] const ReportCB& report) = 0;

  protected:
    Dest() = default;
};
struct Bot : public Dest
{
    size_t id{};
    std::optional<size_t> value;
    Dest* low{};
    Dest* high{};

    Bot() = default;

    void accept(size_t v, [[maybe_unused]] const ReportCB& report) override
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
    std::vector<size_t> values;

    Output() = default;

    void accept(size_t v, [[maybe_unused]] const ReportCB& report) override
    {
        values.push_back(v);
    }
};

struct Input
{
    size_t value;
    Dest* dest;
};

Solution simulate(const std::vector<std::string>& descs)
{
    size_t part1{};
    auto report = [&part1](size_t bot, size_t low, size_t high)
    {
        constexpr size_t LOW = 17;
        constexpr size_t HIGH = 61;
        if (low == LOW && high == HIGH)
        {
            part1 = bot;
        }
    };
    std::map<size_t, Bot> bots;
    std::map<size_t, Output> outputs;
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
            auto id = std::stoul(std::string(m.get<BOT_ID>()));
            auto& bot = bots[id];
            bot.id = id;

            auto isBot = std::string(m.get<BOT_LOW_IS_BOT>()) == "bot";
            auto destId = std::stoul(std::string(m.get<BOT_LOW_ID>()));
            bot.low =
                isBot ? static_cast<Dest*>(&bots[destId]) : static_cast<Dest*>(&outputs[destId]);
            isBot = std::string(m.get<BOT_HIGH_IS_BOT>()) == "bot";
            destId = std::stoul(std::string(m.get<BOT_HIGH_ID>()));
            bot.high =
                isBot ? static_cast<Dest*>(&bots[destId]) : static_cast<Dest*>(&outputs[destId]);
        }
        else if (auto m = ctre::match<INPUT_PAT>(desc))
        {
            auto val = std::stoul(std::string(m.get<INPUT_VAL>()));
            auto id = std::stoul(std::string(m.get<INPUT_ID>()));
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

    using namespace std::views;

    auto firstThreeOutputs = iota(0UL, 3UL) | transform([&outputs](auto i) { return outputs[i]; });
    if (std::ranges::any_of(firstThreeOutputs, [](const auto& o) { return o.values.size() != 1; }))
    {
        throw std::runtime_error("Outputs 0-2 did not each have a single element");
    }
    auto part2 =
        std::ranges::fold_left(firstThreeOutputs | transform([](auto o) { return o.values[0]; }),
                               1UL, std::multiplies<>());
    return {part1, part2};
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto descs = readAllLines(input);
    return simulate(descs);
}
} // namespace aoc
