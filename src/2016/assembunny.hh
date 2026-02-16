
#include <stdexcept>
#include <utility>
#include <vector>

namespace aoc::Assembunny
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
        OUT,
        ADD,
        MUL
    };
    static constexpr Type toggleType(Type t)
    {
        switch (t)
        {
        case Type::INC:
            return Type::DEC;
        case Type::DEC:
        case Type::TGL:
        case Type::OUT:
            return Type::INC;
        case Type::CPY:
            return Type::JNZ;
        case Type::JNZ:
            return Type::CPY;
        case Type::ADD:
        case Type::MUL:
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

std::istream& operator>>(std::istream& is, Instruction& ci);

struct Machine
{
    register_t a{};
    register_t b{};
    register_t c{};
    register_t d{};
    register_t z{};
    std::vector<Instruction> program;
    std::vector<Instruction> optimized;
    std::vector<register_t> out;
    int ip{};

    Machine(const std::vector<Instruction>& program) : program{program}
    {
        optimize();
    }

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

    void step()
    {
        optimized[ip].apply(*this);
    }

    void run()
    {
        while (0 <= ip && ip < std::ssize(optimized))
        {
            step();
        }
    }

    void reset(register_t a = 0, register_t b = 0, register_t c = 0, register_t d = 0)
    {
        this->out.clear();
        this->a = a;
        this->b = b;
        this->c = c;
        this->d = d;
        ip = 0;
    }
};
} // namespace aoc::Assembunny
