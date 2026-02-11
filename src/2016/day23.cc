#include "aoc.hh"
#include <variant>

/* https://adventofcode.com/2016/day/12
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 23;

namespace
{
using register_t = int;
using operand = std::variant<char, register_t>;

struct Machine;
struct IncInstruction;
struct DecInstruction;
struct JnzInstruction;
struct CpyInstruction;
struct TglInstruction;

using Instruction =
    std::variant<CpyInstruction, IncInstruction, DecInstruction, JnzInstruction, TglInstruction>;

struct IncInstruction
{
    operand dest;
    void apply(Machine& cm) const;
    Instruction toggle();
};

struct DecInstruction
{
    operand dest;
    void apply(Machine& cm) const;
    Instruction toggle();
};

struct TglInstruction
{
    operand dest;
    void apply(Machine& cm) const;
    Instruction toggle();
};

struct CpyInstruction
{
    operand src;
    operand dest;
    void apply(Machine& cm) const;
    Instruction toggle();
};

struct JnzInstruction
{
    operand src;
    operand dest;
    void apply(Machine& cm) const;
    Instruction toggle();
};

struct Machine
{
    register_t a{};
    register_t b{};
    register_t c{};
    register_t d{};
    register_t z{};
    std::vector<Instruction> program;
    int ip{};

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
        default:
            throw std::invalid_argument{"no such register"};
        }
    }

    register_t& reference(operand o)
    {
        if (std::holds_alternative<char>(o))
        {
            return reg(std::get<char>(o));
        }
        return z;
    }

    register_t value(operand o)
    {
        if (std::holds_alternative<char>(o))
        {
            return reg(std::get<char>(o));
        }
        return std::get<register_t>(o);
    }

    void run()
    {
        while (ip < std::ssize(program))
        {
            // copy so that tgl can modify program
            auto inst = program[ip];
            std::visit([this](auto&& arg) { arg.apply(*this); }, inst);
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

void CpyInstruction::apply(Machine& cm) const
{

    cm.reference(dest) = cm.value(src);
    ++cm.ip;
}

void IncInstruction::apply(Machine& cm) const
{
    ++cm.reference(dest);
    ++cm.ip;
}

void DecInstruction::apply(Machine& cm) const
{
    --cm.reference(dest);
    ++cm.ip;
}

void JnzInstruction::apply(Machine& cm) const
{
    if (cm.value(src) != 0)
    {
        cm.ip += cm.value(dest);
    }
    else
    {
        ++cm.ip;
    }
}

void TglInstruction::apply(Machine& cm) const
{
    auto addr = cm.ip + cm.value(dest);
    if (addr >= 0 && addr < std::ssize(cm.program))
    {
        cm.program[addr] = std::visit([](auto instr) { return instr.toggle(); }, cm.program[addr]);
    }
    ++cm.ip;
}

Instruction DecInstruction::toggle()
{
    return IncInstruction{dest};
}
Instruction IncInstruction::toggle()
{
    return DecInstruction{dest};
}
Instruction TglInstruction::toggle()
{
    return IncInstruction{dest};
}
Instruction CpyInstruction::toggle()
{
    return JnzInstruction{.src = src, .dest = dest};
}
Instruction JnzInstruction::toggle()
{
    return CpyInstruction{.src = src, .dest = dest};
}

std::istream& operator>>(std::istream& is, operand& op)
{
    char reg{};
    register_t val{};
    is >> reg;
    if (std::isalpha(reg))
    {
        op = reg;
    }
    else
    {
        is.putback(reg);
        is >> val;
        op = val;
    }
    return is;
}

std::istream& operator>>(std::istream& is, Instruction& ci)
{
    std::string buf;
    if (!(is >> buf))
    {
        return is;
    }
    if (buf == "cpy")
    {
        CpyInstruction instr;
        is >> instr.src;
        is >> instr.dest;
        ci = instr;
        return is;
    }
    if (buf == "inc")
    {
        IncInstruction instr;
        is >> instr.dest;
        ci = instr;
        return is;
    }
    if (buf == "dec")
    {
        DecInstruction instr;
        is >> instr.dest;
        ci = instr;
        return is;
    }
    if (buf == "jnz")
    {
        JnzInstruction instr;
        is >> instr.src;
        is >> instr.dest;
        ci = instr;
        return is;
    }
    if (buf == "tgl")
    {
        TglInstruction instr;
        is >> instr.dest;
        ci = instr;
        return is;
    }
    is.setstate(std::ios_base::failbit);
    return is;
}

} // namespace

template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    std::vector<Instruction> program;
    for (Instruction ci; input >> ci;)
    {
        program.push_back(ci);
    }
    constexpr auto PART1_INPUT = 7;
    constexpr auto PART2_INPUT = 12;
    Machine cm{.a = PART1_INPUT, .program = program};
    cm.run();
    auto part1 = cm.a;
    cm = {.a = PART2_INPUT, .program = std::move(program)};
    cm.run();

    return {part1, cm.a};
}
} // namespace aoc
