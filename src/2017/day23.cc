#include "aoc.hh"
#include <deque>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

/* https://adventofcode.com/2017/day/23
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 23;
using Solution = Solution_t<YEAR, DAY>;

namespace Interpreter23
{
using register_t = int64_t;
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
    // NOLINTNEXTLINE (performance-enum-size)
    enum class Type
    {
        SND,
        SET,
        ADD,
        SUB,
        MUL,
        MOD,
        RCV,
        JNZ,
        JGZ,
        COUNT
    };
    Type t{};
    Operand first;
    Operand second;

    Instruction(Type t = Type::SND, Operand first = {}, Operand second = {})
        : t{t}, first{first}, second{second}
    {
    }

    bool operator==(const Instruction& o) const = default;
    void apply(Machine& m) const;
};

std::istream& operator>>(std::istream& is, Instruction& ci);

struct Machine
{
    static constexpr auto NUM_REG = 26;
    std::array<register_t, NUM_REG> registers{};
    std::vector<Instruction> program;
    std::deque<register_t> sent;
    std::deque<register_t>* receiveQueue{};
    std::optional<register_t> recovered;
    bool blocked{};
    std::vector<int> instructionCounts;
    ssize_t ip{};

    Machine(std::vector<Instruction> program)
        : program{std::move(program)}, instructionCounts(static_cast<int>(Instruction::Type::COUNT))
    {
    }

    register_t& reg(char r)
    {
        return registers.at(r - 'a');
    }

    void step()
    {
        if (0 <= ip && ip < std::ssize(program))
        {
            program[ip].apply(*this);
        }
        else
        {
            blocked = true;
        }
    }

    void run()
    {
        while (!blocked)
        {
            step();
        }
    }

    int& instructionCount(Instruction::Type t)
    {
        auto index = static_cast<int>(t);
        return instructionCounts[index];
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
    ++cm.instructionCount(t);
    switch (t)
    {
    case Type::SND:
        cm.sent.push_back(first.value(cm));
        break;
    case Type::SET:
        first.reference(cm) = second.value(cm);
        break;
    case Type::ADD:
        first.reference(cm) += second.value(cm);
        break;
    case Type::SUB:
        first.reference(cm) -= second.value(cm);
        break;
    case Type::MUL:
        first.reference(cm) *= second.value(cm);
        break;
    case Type::MOD:
        if (first.value(cm) < 0 || second.value(cm) < 0)
        {
            throw std::invalid_argument("Negative mod");
        }
        first.reference(cm) %= second.value(cm);
        break;
    case Type::RCV:
        if (cm.receiveQueue == nullptr)
        {
            if (first.value(cm) != 0)
            {
                if (cm.sent.empty())
                {
                    throw std::invalid_argument("Receive before send");
                }
                cm.recovered = cm.sent.back();
            }
        }
        else
        {
            if (cm.receiveQueue->empty())
            {
                cm.blocked = true;
                return;
            }
            cm.blocked = false;
            first.reference(cm) = cm.receiveQueue->front();
            cm.receiveQueue->pop_front();
        }
        break;
    case Type::JNZ:
        if (first.value(cm) != 0)
        {
            cm.ip += second.value(cm);
            return;
        }
        break;
    case Type::JGZ:
        if (first.value(cm) > 0)
        {
            cm.ip += second.value(cm);
            return;
        }
        break;
    case Type::COUNT:
        throw std::logic_error("Instruction has COUNT");
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
    if (buf == "snd")
    {
        ci.t = Instruction::Type::SND;
    }
    else if (buf == "set")
    {
        ci.t = Instruction::Type::SET;
    }
    else if (buf == "add")
    {
        ci.t = Instruction::Type::ADD;
    }
    else if (buf == "sub")
    {
        ci.t = Instruction::Type::SUB;
    }
    else if (buf == "mul")
    {
        ci.t = Instruction::Type::MUL;
    }
    else if (buf == "mod")
    {
        ci.t = Instruction::Type::MOD;
    }
    else if (buf == "rcv")
    {
        ci.t = Instruction::Type::RCV;
    }
    else if (buf == "jnz")
    {
        ci.t = Instruction::Type::JNZ;
    }
    else if (buf == "jgz")
    {
        ci.t = Instruction::Type::JGZ;
    }
    else
    {
        is.setstate(std::ios_base::failbit);
        return is;
    }
    is >> ci.first;
    if (ci.t != Instruction::Type::SND && ci.t != Instruction::Type::RCV)
    {
        is >> ci.second;
    }
    return is;
}
auto part2()
{
    constexpr auto B = 108'400;
    constexpr auto STEP = 17;
    constexpr auto C = B + STEP * 1000;
    auto res = 0;
    for (auto b = B; b <= C; b += STEP)
    {
        for (auto t = 2; t * t <= b; ++t)
        {
            if (b % t == 0)
            {
                ++res;
                break;
            }
        }
    }
    return res;
}
} // namespace Interpreter23
template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    using namespace Interpreter23;
    auto program = readAll<Instruction>(input);
    auto m0 = Machine{program};
    m0.run();
    auto part1 = m0.instructionCount(Instruction::Type::MUL);

    auto m1 = Machine{std::move(program)};
    // m1.reg('a') = 1;
    // m1.run();
    // auto part2 = m1.reg('h');
    return {part1, part2()};
}
} // namespace aoc
