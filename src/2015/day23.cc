#include "aoc.hh"
#include <variant>

/* https://adventofcode.com/2015/day/23
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 23;

namespace
{
using register_t = uint64_t;

struct CollatzMachine;
struct IncInstruction
{
    char reg;
    void apply(CollatzMachine& cm) const;
};

struct HlfInstruction
{
    char reg;
    void apply(CollatzMachine& cm) const;
};

struct TplInstruction
{
    char reg;
    void apply(CollatzMachine& cm) const;
};

struct JmpInstruction
{
    ssize_t offset;
    void apply(CollatzMachine& cm) const;
};

struct JieInstruction
{
    ssize_t offset;
    char reg;
    void apply(CollatzMachine& cm) const;
};

struct JioInstruction
{
    ssize_t offset;
    char reg;
    void apply(CollatzMachine& cm) const;
};

using Instruction = std::variant<IncInstruction, HlfInstruction, TplInstruction, JmpInstruction,
                                 JieInstruction, JioInstruction>;
struct CollatzMachine
{
    register_t a{};
    register_t b{};
    std::vector<Instruction> program;
    size_t ip{};

    register_t& reg(char c)
    {
        switch (c)
        {
        case 'a':
            return a;
        case 'b':
            return b;
        default:
            throw std::invalid_argument{"no such register"};
        }
    }

    void run()
    {
        while (ip < program.size())
        {
            const auto& inst = program[ip];
            std::visit([this](auto&& arg) { arg.apply(*this); }, inst);
        }
    }

    void reset(register_t a = 0, register_t b = 0)
    {
        this->a = a;
        this->b = b;
        ip = 0;
    }
};

void IncInstruction::apply(CollatzMachine& cm) const
{
    ++cm.reg(reg);
    ++cm.ip;
}
void HlfInstruction::apply(CollatzMachine& cm) const
{
    cm.reg(reg) >>= 1;
    ++cm.ip;
}
void TplInstruction::apply(CollatzMachine& cm) const
{
    cm.reg(reg) *= 3;
    ++cm.ip;
}
void JmpInstruction::apply(CollatzMachine& cm) const
{
    cm.ip += offset;
}
void JieInstruction::apply(CollatzMachine& cm) const
{
    if (cm.reg(reg) % 2 == 0)
    {
        cm.ip += offset;
    }
    else
    {
        ++cm.ip;
    }
}
void JioInstruction::apply(CollatzMachine& cm) const
{
    if (cm.reg(reg) == 1)
    {
        cm.ip += offset;
    }
    else
    {
        ++cm.ip;
    }
}

std::istream& operator>>(std::istream& is, Instruction& ci)
{
    std::string buf;
    if (!(is >> buf))
    {
        return is;
    }
    char c{};
    if (buf == "inc")
    {
        is >> c;
        ci = IncInstruction{c};
        return is;
    }
    if (buf == "hlf")
    {
        is >> c;
        ci = HlfInstruction{c};
        return is;
    }
    if (buf == "tpl")
    {
        is >> c;
        ci = TplInstruction{c};
        return is;
    }

    ssize_t offset{};
    if (buf == "jmp")
    {
        is >> offset;
        ci = JmpInstruction{offset};
        return is;
    }
    if (buf == "jie")
    {
        is >> c;
        is.ignore();
        is >> offset;
        ci = JieInstruction{.offset = offset, .reg = c};
        return is;
    }

    if (buf == "jio")
    {
        is >> c;
        is.ignore();
        is >> offset;
        ci = JioInstruction{.offset = offset, .reg = c};
        return is;
    }

    is.setstate(std::ios_base::failbit);
    return is;
}

} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    std::vector<Instruction> program;
    for (Instruction ci; input >> ci;)
    {
        program.push_back(ci);
    }

    CollatzMachine cm{.program = std::move(program)};
    cm.run();
    auto part1 = cm.b;
    cm.reset(1);
    cm.run();

    return {part1, cm.b};
}
} // namespace aoc
