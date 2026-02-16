#include "aoc.hh"
#include <ranges>
#include <utility>

/* https://adventofcode.com/2016/day/23
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 23;

namespace
{
using register_t = int;

struct Machine;
struct Instruction;
struct Operand
{
    char reg{};
    register_t imm{};

    Operand(char reg) : reg{reg} {}
    Operand(int imm) : imm{imm} {}
    Operand() = default;

    bool operator==(const Operand& o) const = default;

    register_t& reference(Machine& cm) const;
    register_t value(Machine& cm) const;
};
struct Instruction
{
    static constexpr auto TIMES_INSTRUCTION_COUNT = 6;
    static constexpr auto PLUS_INSTRUCTION_COUNT = 3;
    // NOLINTNEXTLINE (performance-enum-size)
    enum class Type
    {
        INC,
        DEC,
        CPY,
        JNZ,
        TGL,
        PLS,
        TIM
    };
    static constexpr Type toggleType(Type t)
    {
        switch (t)
        {
        case Type::INC:
            return Type::DEC;
        case Type::DEC:
        case Type::TGL:
            return Type::INC;
        case Type::CPY:
            return Type::JNZ;
        case Type::JNZ:
            return Type::CPY;
        case Type::PLS:
        case Type::TIM:
            throw std::logic_error("Toggle performed on pseudo instruction");
        }
        std::unreachable();
    }
    Type t{};
    Operand first;
    Operand second;
    Operand third;
    Operand fourth;

    Instruction(Type t = Type::INC, Operand first = {}, Operand second = {}, Operand third = {},
                Operand fourth = {})
        : t{t}, first{first}, second{second}, third{third}, fourth{fourth}
    {
    }

    bool operator==(const Instruction& o) const = default;

    void toggle()
    {
        t = toggleType(t);
    }

    void apply(Machine& m) const;
};

[[maybe_unused]] void apply(Instruction i, Machine& cm)
{
    i.apply(cm);
}

struct Machine
{
    register_t a{};
    register_t b{};
    register_t c{};
    register_t d{};
    register_t z{};
    std::vector<Instruction> program;
    std::vector<Instruction> optimized;
    int ip{};

    Machine(const std::vector<Instruction>& program) : program{program} {}

    register_t& reg(char r)
    {
        switch (r)
        {
        case 'a':
            return a;
        case 'b':
            return b;
        case 'c':
            return c;
        case 'd':
            return d;
        case '\0':
            return z;
        default:
            throw std::invalid_argument{"no such register"};
        }
    }

    void optimize();

    void run()
    {
        optimize();
        // the loop is rather small, 0 <= ip && ip < std::ssize(program) makes runtime 15% longer!
        while (static_cast<size_t>(ip) < optimized.size())
        {
            apply(optimized[ip], *this);
        }
    }

    void reset(register_t a = 0, register_t b = 0, register_t c = 0, register_t d = 0)
    {
        this->a = a;
        this->b = b;
        this->c = c;
        this->d = d;
        ip = 0;
    }
};

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

void Machine::optimize()
{
    optimized = program;
    using std::views::drop;
    auto& o = optimized;
    for (auto [a, b, c, d, e, f] :
         std::views::zip(o, o | drop(1), o | drop(2), o | drop(3), o | drop(4), o | drop(5)))
    {
        // This becomes 1st += 2nd*3rd, clobbering 3rd and 4th
        // cpy 2nd 4th
        // inc 1st
        // dec 4th
        // jnz 4th -2
        // dec 3rd
        // jnz 3rd -5
        if (a.t == Instruction::Type::CPY && b.t == Instruction::Type::INC &&
            c.t == Instruction::Type::DEC && d.t == Instruction::Type::JNZ &&
            d.second.imm == -(Instruction::PLUS_INSTRUCTION_COUNT - 1) &&
            e.t == Instruction::Type::DEC && f.t == Instruction::Type::JNZ &&
            f.second.imm == -(Instruction::TIMES_INSTRUCTION_COUNT - 1))
        {
            Instruction mul{Instruction::Type::TIM, b.first, a.first, e.first, a.second};
            if (mul.first == mul.second || mul.first == mul.third || mul.first == mul.fourth ||
                mul.second == mul.third || mul.second == mul.fourth || mul.third == mul.fourth)
            {
                continue;
            }
            if (c.first != mul.fourth || d.first != mul.fourth || f.first != mul.third)
            {
                continue;
            }
            a = mul;
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
    case Type::TIM:
        first.reference(cm) += second.value(cm) * third.value(cm);
        fourth.reference(cm) = 0;
        third.reference(cm) = 0;
        cm.ip += TIMES_INSTRUCTION_COUNT;
        return;
    case Type::PLS:
        first.reference(cm) += second.value(cm);
        second.reference(cm) = 0;
        cm.ip += PLUS_INSTRUCTION_COUNT;
        return;
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

} // namespace

template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    auto flatProgram = readAll<Instruction>(input);
    constexpr auto PART1_INPUT = 7;
    constexpr auto PART2_INPUT = 12;
    Machine cm{flatProgram};
    cm.reset(PART1_INPUT);
    cm.run();
    auto part1 = cm.a;
    cm = {flatProgram};
    cm.reset(PART2_INPUT);
    cm.run();
    return {part1, cm.a};
}
} // namespace aoc
