#include "aoc.hh"
#include <utility>

/* https://adventofcode.com/2016/day/25
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 25;

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
        TGL,
        OUT
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
        case Type::OUT:
            return Type::INC;
        }
        std::unreachable();
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
    int ip{};
    std::vector<register_t> out;
    std::vector<Instruction> program;

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

    void step()
    {
        apply(program[ip], *this);
    }
    void run()
    {
        while (static_cast<size_t>(ip) < program.size())
        {
            step();
        }
    }

    void reset(register_t a = 0, register_t b = 0, register_t c = 0, register_t d = 0)
    {
        this->a = a;
        this->b = b;
        this->c = c;
        this->d = d;
        out.clear();
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
    {
        auto addr = cm.ip + src.value(cm);
        if (addr >= 0 && addr < std::ssize(cm.program))
        {
            cm.program[addr].toggle();
        }
        break;
    }
    case Type::OUT:
        cm.out.push_back(src.value(cm));
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
        ci.t = Instruction::Type ::OUT;
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
