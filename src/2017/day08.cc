#include "aoc.hh"
#include <algorithm>
#include <ctre.hpp>
#include <map>
#include <ranges>
#include <utility>

/* https://adventofcode.com/2017/day/8
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 8;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
namespace Parse
{
constexpr auto PAT =
    ctll::fixed_string(R"((\w*)\s+(inc|dec)\s+(-?\d+)\s+if\s+(\w*)\s+(==|!=|>=?|<=?)\s+(-?\d+))");
constexpr auto REG = 1;
constexpr auto OP = 2;
constexpr auto AMT = 3;
constexpr auto COND_REG = 4;
constexpr auto COND_OP = 5;
constexpr auto COND_AMT = 6;
} // namespace Parse

struct Instruction
{
    enum class Op
    {
        INC,
        DEC
    };
    enum class CondOp
    {
        EQ,
        NEQ,
        GT,
        GE,
        LT,
        LE,
        UNKNOWN
    };

    std::string reg;
    Op op;
    int amt;
    std::string condReg;
    CondOp condOp;
    int condAmt;

    static CondOp toCondOp(std::string_view text);
    int apply(auto& registerFile) const;
};

Instruction::CondOp Instruction::toCondOp(std::string_view text)
{
    if (text == "==")
    {
        return CondOp::EQ;
    }
    if (text == "!=")
    {
        return CondOp::NEQ;
    }
    if (text == ">")
    {
        return CondOp::GT;
    }
    if (text == ">=")
    {
        return CondOp::GE;
    }
    if (text == "<")
    {
        return CondOp::LT;
    }
    if (text == "<=")
    {
        return CondOp::LE;
    }
    return CondOp::UNKNOWN;
}
int Instruction::apply(auto& registerFile) const
{
    auto test = registerFile[condReg];
    bool cond{};
    switch (condOp)
    {

    case CondOp::EQ:
        cond = test == condAmt;
        break;
    case CondOp::NEQ:
        cond = test != condAmt;
        break;
    case CondOp::GT:
        cond = test > condAmt;
        break;
    case CondOp::GE:
        cond = test >= condAmt;
        break;
    case CondOp::LT:
        cond = test < condAmt;
        break;
    case CondOp::LE:
        cond = test <= condAmt;
        break;
    case CondOp::UNKNOWN:
        std::unreachable();
    }
    if (cond)
    {
        registerFile[reg] += op == Op::INC ? amt : -amt;
    }
    return registerFile[reg];
}

[[maybe_unused]] std::istream& operator>>(std::istream& is, Instruction& instr)
{
    std::string text;
    std::getline(is, text);
    {
        using namespace Parse;
        if (auto m = ctre::match<PAT>(text))
        {
            instr.reg = m.get<REG>().str();
            instr.op = m.get<OP>() == "inc" ? Instruction::Op::INC : Instruction::Op::DEC;
            instr.amt = m.get<AMT>().to_number();
            instr.condReg = m.get<COND_REG>().str();
            instr.condOp = Instruction::toCondOp(m.get<COND_OP>());
            instr.condAmt = m.get<COND_AMT>().to_number();
        }
    }
    return is;
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto program = readAll<Instruction>(input);
    std::map<std::string, int> registerFile;
    auto max = int{};
    for (const auto& instr : program)
    {
        max = std::max(max, instr.apply(registerFile));
    }

    auto values = registerFile | std::views::values;
    return {std::ranges::max(values), max};
}
} // namespace aoc
