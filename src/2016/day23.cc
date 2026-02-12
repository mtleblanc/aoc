#include "aoc.hh"

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

    register_t& reference(Machine& cm) const;
    register_t value(Machine& cm) const;
};
struct Instruction
{
    // NOLINTNEXTLINE (performance-enum-size)
    enum class Type
    {
        INC,
        DEC,
        CPY,
        JNZ,
        TGL
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
        }
    }
    Type t{};
    Operand src;
    Operand dst;

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
        case '\0':
            return z;
        default:
            throw std::invalid_argument{"no such register"};
        }
    }

    void run()
    {
        // the loop is rather small, 0 <= ip && ip < std::ssize(program) makes runtime 15% longer!
        while (static_cast<size_t>(ip) < program.size())
        {
            apply(program[ip], *this);
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

void Instruction::apply(Machine& cm) const
{
    switch (t)
    {
    case Type::INC:
        src.reference(cm)++;
        break;
    case Type::DEC:
        src.reference(cm)--;
        break;
    case Type::CPY:
        dst.reference(cm) = src.value(cm);
        break;
    case Type::JNZ:
        if (src.value(cm) != 0)
        {
            cm.ip += dst.value(cm);
            return;
        }
        break;
    case Type::TGL:
        auto addr = cm.ip + src.value(cm);
        if (addr >= 0 && addr < std::ssize(cm.program))
        {
            cm.program[addr].toggle();
        }
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
    is >> ci.src;
    if (ci.t == Instruction::Type::JNZ || ci.t == Instruction::Type::CPY)
    {
        is >> ci.dst;
    }
    return is;
}

} // namespace

template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    auto flatProgram = readAll<Instruction>(input);
    constexpr auto PART1_INPUT = 7;
    // set to 12 for correct answer
    constexpr auto PART2_INPUT = 7;
    Machine cm{.a = PART1_INPUT, .program = flatProgram};
    cm.run();
    auto part1 = cm.a;
    cm = {.a = PART2_INPUT, .program = std::move(flatProgram)};
    cm.run();
    return {part1, cm.a};
}
} // namespace aoc
