#include "aoc.hh"
#include "assembunny.hh"
#include <optional>
#include <ranges>

/* https://adventofcode.com/2016/day/25
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 25;

namespace Assembunny
{

register_t& Operand::reference(Machine& cm) const
{
    return cm.reg(reg);
}
register_t Operand::value(Machine& cm) const
{
    if (reg)
    {
        return cm.reg(reg);
    }
    return imm;
}

// Detect an add loop pattern:
//   inc dst
//   dec src
//   jnz src -2      (inner loop: dst += src)
// Result: dst += src, with src zeroed
std::optional<Instruction> detectAdd(const Instruction& i0, const Instruction& i1,
                                     const Instruction& i2)
{
    using Type = Instruction::Type;
    if (i0.t != Type::INC || i1.t != Type::DEC || i2.t != Type::JNZ)
    {
        return {};
    }
    if (i2.second.imm != -(Instruction::PLUS_INSTRUCTION_COUNT - 1))
    {
        return {};
    }
    if (i2.first != i1.first)
    {
        return {};
    }
    auto dst = i0.first;
    auto src = i1.first;
    if (dst == src || src.reg == '\0')
    {
        return {};
    }
    return Instruction{Type::ADD, src, dst};
}

// Detect a multiply loop pattern:
//   cpy src tmp
//   add tmp dst
//   __
//   __
//   dec cnt
//   jnz cnt -5      (outer loop: repeat cnt times)
// Result: dst += src * cnt, with tmp and cnt zeroed
std::optional<Instruction> detectMultiply(const Instruction& i0, const Instruction& i1,
                                          [[maybe_unused]] const Instruction& i2,
                                          [[maybe_unused]] const Instruction& i3,
                                          const Instruction& i4, const Instruction& i5)
{
    using Type = Instruction::Type;
    if (i0.t != Type::CPY || i1.t != Type::ADD || i4.t != Type::DEC || i5.t != Type::JNZ)
    {
        return {};
    }

    if (i5.second.imm != -(Instruction::TIMES_INSTRUCTION_COUNT - 1))
    {
        return {};
    }

    auto dst = i1.second;
    auto src = i0.first;
    auto cnt = i4.first;
    auto tmp = i0.second;

    // Verify inner/outer loop variables match
    if (i1.first != tmp || i5.first != cnt)
    {
        return {};
    }

    // All operands must be distinct
    if (dst == src || dst == cnt || dst == tmp || src == cnt || src == tmp || cnt == tmp)
    {
        return {};
    }

    return Instruction{Type::MUL, src, cnt, dst, tmp};
}

void Machine::optimize()
{
    optimized = program;
    using std::views::drop;
    auto& o = optimized;
    for (auto [i0, i1, i2] : std::views::zip(o, o | drop(1), o | drop(2)))
    {
        if (auto add = detectAdd(i0, i1, i2))
        {
            i0 = *add;
        }
    }
    for (auto [i0, i1, i2, i3, i4, i5] :
         std::views::zip(o, o | drop(1), o | drop(2), o | drop(3), o | drop(4), o | drop(5)))
    {
        if (auto mul = detectMultiply(i0, i1, i2, i3, i4, i5))
        {
            i0 = *mul;
        }
    }
}

void Instruction::apply(Machine& cm) const
{
    switch (t)
    {
    case Type::INC:
        first.reference(cm)++;
        break;
    case Type::DEC:
        first.reference(cm)--;
        break;
    case Type::CPY:
        second.reference(cm) = first.value(cm);
        break;
    case Type::JNZ:
        if (first.value(cm) != 0)
        {
            cm.ip += second.value(cm);
            return;
        }
        break;
    case Type::TGL:
    {
        auto addr = cm.ip + first.value(cm);
        if (addr >= 0 && addr < std::ssize(cm.program))
        {
            cm.program[addr].toggle();
            cm.optimize();
        }
        break;
    }
    case Type::MUL:
        third.reference(cm) += first.value(cm) * second.value(cm);
        fourth.reference(cm) = 0;
        second.reference(cm) = 0;
        cm.ip += TIMES_INSTRUCTION_COUNT;
        return;
    case Type::ADD:
        second.reference(cm) += first.value(cm);
        first.reference(cm) = 0;
        cm.ip += PLUS_INSTRUCTION_COUNT;
        return;
    case Type::OUT:
        cm.out.push_back(first.value(cm));
        break;
    }
    ++cm.ip;
}

[[maybe_unused]] std::istream& operator>>(std::istream& is, Operand& op)
{
    char reg{};
    is >> reg;
    if (std::isalpha(reg))
    {
        op.reg = reg;
    }
    else
    {
        op.reg = '\0';
        is.putback(reg);
        is >> op.imm;
    }
    return is;
}

[[maybe_unused]] std::istream& operator>>(std::istream& is, Instruction& ci)
{
    std::string buf;
    if (!(is >> buf))
    {
        return is;
    }
    if (buf == "cpy")
    {
        ci.t = Instruction::Type::CPY;
    }
    else if (buf == "inc")
    {
        ci.t = Instruction::Type::INC;
    }
    else if (buf == "dec")
    {
        ci.t = Instruction::Type::DEC;
    }
    else if (buf == "jnz")
    {
        ci.t = Instruction::Type::JNZ;
    }
    else if (buf == "tgl")
    {
        ci.t = Instruction::Type::TGL;
    }
    else if (buf == "out")
    {
        ci.t = Instruction::Type::OUT;
    }
    else
    {
        is.setstate(std::ios_base::failbit);
        return is;
    }
    is >> ci.first;
    if (ci.t == Instruction::Type::JNZ || ci.t == Instruction::Type::CPY)
    {
        is >> ci.second;
    }
    return is;
}
} // namespace Assembunny

template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    using namespace Assembunny;
    auto flatProgram = readAll<Instruction>(input);
    // we should really detect state repeats, but the program is printing out binary repeatedly and
    // this is large enough to check
    auto expected = std::vector{0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
    Machine cm{flatProgram};
    for (register_t a{1};; ++a)
    {
        cm.reset(a);
        while (cm.out.size() < expected.size())
        {
            cm.step();
        }
        if (expected == cm.out)
        {
            return {a};
        }
    }
}
} // namespace aoc
