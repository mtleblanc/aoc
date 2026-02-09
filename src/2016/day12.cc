#include "aoc.hh"
#include <variant>

/* https://adventofcode.com/2016/day/12
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 12;

namespace
{
using register_t = int;

struct Machine;

struct CpyImmInstruction
{
    register_t imm;
    char dest;
    void apply(Machine& cm) const;
};

struct CpyRegInstruction
{
    char src;
    char dest;
    void apply(Machine& cm) const;
};

struct IncInstruction
{
    char reg;
    void apply(Machine& cm) const;
};

struct DecInstruction
{
    char reg;
    void apply(Machine& cm) const;
};

struct JnzInstruction
{
    int offset;
    char reg;
    void apply(Machine& cm) const;
};

struct JmpInstruction
{
    int offset;
    void apply(Machine& cm) const;
};

struct NopInstruction
{
    void apply(Machine& cm) const;
};

using Instruction = std::variant<CpyImmInstruction, CpyRegInstruction, IncInstruction,
                                 DecInstruction, JnzInstruction, JmpInstruction, NopInstruction>;
struct Machine
{
    register_t a{};
    register_t b{};
    register_t c{};
    register_t d{};
    std::vector<Instruction> program;
    size_t ip{};

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

    void run()
    {
        while (ip < program.size())
        {
            const auto& inst = program[ip];
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

void CpyImmInstruction::apply(Machine& cm) const
{
    cm.reg(dest) = imm;
    ++cm.ip;
}

void CpyRegInstruction::apply(Machine& cm) const
{
    cm.reg(dest) = cm.reg(src);
    ++cm.ip;
}

void IncInstruction::apply(Machine& cm) const
{
    ++cm.reg(reg);
    ++cm.ip;
}

void DecInstruction::apply(Machine& cm) const
{
    --cm.reg(reg);
    ++cm.ip;
}

void JnzInstruction::apply(Machine& cm) const
{
    if (cm.reg(reg) != 0)
    {
        cm.ip += offset;
    }
    else
    {
        ++cm.ip;
    }
}

void JmpInstruction::apply(Machine& cm) const
{
    cm.ip += offset;
}

// NOLINTNEXTLINE (readability-convert-member-functions-to-static)
void NopInstruction::apply(Machine& cm) const
{
    ++cm.ip;
}

std::istream& operator>>(std::istream& is, Instruction& ci)
{
    std::string buf;
    if (!(is >> buf))
    {
        return is;
    }
    char dest{};
    char src{};
    int offset{};
    register_t imm{};
    if (buf == "cpy")
    {
        is >> src;
        if (std::isalpha(src))
        {
            is >> dest;
            ci = CpyRegInstruction{.src = src, .dest = dest};
            return is;
        }
        is.putback(src);
        is >> imm;
        is >> dest;
        ci = CpyImmInstruction{.imm = imm, .dest = dest};
        return is;
    }
    if (buf == "inc")
    {
        is >> dest;
        ci = IncInstruction{dest};
        return is;
    }
    if (buf == "dec")
    {
        is >> dest;
        ci = DecInstruction{dest};
        return is;
    }
    if (buf == "jnz")
    {
        is >> dest;
        if (std::isalpha(dest))
        {
            is >> offset;
            ci = JnzInstruction{.offset = offset, .reg = dest};
            return is;
        }
        is.putback(dest);
        is >> imm;
        is >> offset;
        if (imm == 0)
        {
            ci = NopInstruction{};
        }
        else
        {
            ci = JmpInstruction{.offset = offset};
        }
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

    Machine cm{.program = std::move(program)};
    cm.run();
    auto part1 = cm.a;
    cm.reset(0, 0, 1, 0);
    cm.run();

    return {part1, cm.a};
}
} // namespace aoc
