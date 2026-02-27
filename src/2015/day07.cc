#include "aoc.hh"
#include <cassert>
#include <map>
#include <optional>
#include <ranges>
#include <string>
#include <utility>

/* https://adventofcode.com/2015/day/7
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 7;

namespace
{
enum class Op : uint8_t
{
    CONSTANT,
    EQ,
    NOT,
    AND,
    OR,
    RSHIFT,
    LSHIFT,
};
template <typename T> struct Wire
{
    std::optional<T> value;
    Op op{Op::CONSTANT};
    Wire* input1{nullptr};
    Wire* input2{nullptr};
    T eval()
    {
        if (value)
        {
            return *value;
        }
        switch (op)
        {
        case Op::CONSTANT:
            assert(false);
            return *value;
        case Op::EQ:
            value = input1->eval();
            return *value;
        case Op::NOT:
            value = ~input1->eval();
            return *value;
        case Op::AND:
            value = input1->eval() & input2->eval();
            return *value;
        case Op::OR:
            value = input1->eval() | input2->eval();
            return *value;
        case Op::LSHIFT:
            value = input1->eval() << input2->eval();
            return *value;
        case Op::RSHIFT:
            value = input1->eval() >> input2->eval();
            return *value;
        }
        std::unreachable();
    }
};

template <typename T> struct Circuit
{
    std::map<std::string, Wire<T>> lut;
    Wire<T>& operator[](std::string key)
    {
        auto [it, inserted] = lut.emplace(key, Wire<T>{});
        auto& [k, w] = *it;
        if (inserted && std::isdigit(key[0]))
        {
            w.value = static_cast<T>(std::stoul(key));
        }
        return w;
    }
    void reset()
    {
        for (auto& wp : lut)
        {
            auto& [_, w] = wp;
            if (w.op != Op::CONSTANT)
            {
                w.value.reset();
            }
        }
    }
};

template <typename T> auto buildCircuit(const std::vector<std::string>& connections)
{
    Circuit<T> lut;
    for (const auto& connection : connections)
    {
        auto parts = std::ranges::to<std::vector>(
            connection | std::views::split(' ') |
            std::views::transform([](auto rng) { return std::string(rng.begin(), rng.end()); }));
        assert(parts[parts.size() - 2] == "->");
        Wire<T>& w = lut[parts.back()];
        if (parts.size() == 3)
        { // a -> b
            w.input1 = &(lut[parts[0]]);
            w.op = Op::EQ;
        }
        else if (parts.size() == 4)
        { // NOT a -> b
            assert(parts[0] == "NOT");
            w.op = Op::NOT;
            w.input1 = &lut[parts[1]];
        }
        else
        { // a OP b -> c
            w.input1 = &lut[parts[0]];
            w.input2 = &lut[parts[2]];
            if (parts[1] == "AND")
            {
                w.op = Op::AND;
            }
            else if (parts[1] == "OR")
            {
                w.op = Op::OR;
            }
            else if (parts[1] == "LSHIFT")
            {
                w.op = Op::LSHIFT;
            }
            else if (parts[1] == "RSHIFT")
            {
                w.op = Op::RSHIFT;
            }
            else
            {
                assert(false);
            }
        }
    }
    return lut;
}
} // namespace

template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    auto lines = readAllLines(input);
    auto circuit = buildCircuit<uint16_t>(lines);
    auto part1 = circuit["a"].eval();
    circuit.reset();
    circuit["b"].value = part1;
    return {part1, circuit["a"].eval()};
}
} // namespace aoc
