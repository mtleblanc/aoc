#include "aoc.hh"
#include <deque>
#include <stdexcept>
#include <vector>

/* https://adventofcode.com/2017/day/18
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 18;
using Solution = Solution_t<YEAR, DAY>;

namespace Interpreter
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
        MUL,
        MOD,
        RCV,
        JGZ
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
    std::vector<register_t> received;
    bool blocked{};
    ssize_t ip{};

    Machine(const std::vector<Instruction>& program) : program{program} {}
    Machine(std::vector<Instruction>&& program) : program{std::move(program)} {}

    register_t& reg(char r)
    {
        return registers.at(r - 'a');
    }

    void optimize();

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

    void reset()
    {
        for (auto& r : registers)
        {
            r = 0;
        }
        sent.clear();
        received.clear();
        receiveQueue = nullptr;
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
    case Type::SND:
        cm.sent.push_back(first.value(cm));
        break;
    case Type::SET:
        first.reference(cm) = second.value(cm);
        break;
    case Type::ADD:
        first.reference(cm) += second.value(cm);
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
        if (first.value(cm) != 0)
        {
            if (cm.receiveQueue == nullptr)
            {
                if (cm.sent.empty())
                {
                    throw std::invalid_argument("Receive before send");
                }
                cm.received.push_back(cm.sent.back());
            }
            else
            {
                if (cm.receiveQueue->empty())
                {
                    cm.blocked = true;
                    return;
                }
                cm.blocked = false;
                cm.received.push_back(cm.receiveQueue->front());
                cm.receiveQueue->pop_front();
            }
        }
        break;
    case Type::JGZ:
        if (first.value(cm) > 0)
        {
            cm.ip += second.value(cm);
            return;
        }
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
} // namespace Interpreter
template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    using namespace Interpreter;
    auto program = readAll<Instruction>(input);
    auto m0 = Machine{program};
    while (m0.received.empty())
    {
        m0.step();
        if (m0.blocked)
        {
            throw std::invalid_argument("Program ended before receiving");
        }
    }
    int part1 = static_cast<int>(m0.received.front());
    m0.reset();
    auto m1 = Machine{std::move(program)};
    m1.reg('p') = 1;
    m1.receiveQueue = &m0.sent;
    m0.receiveQueue = &m1.sent;
    while (!m0.blocked || !m1.blocked)
    {
        m0.step();
        m1.step();
    }
    int part2 = static_cast<int>(std::ssize(m1.sent) + std::ssize(m0.received));
    return {part1, part2};
}
} // namespace aoc
