#include "aoc.hh"
#include "util.hh"
#include <algorithm>
#include <stdexcept>

/* https://adventofcode.com/2017/day/16
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 16;

template <> struct SolutionType<YEAR, DAY>
{
    using type = aoc::StringSolution;
};

using Solution = Solution_t<YEAR, DAY>;

namespace
{
constexpr auto N = 16;
struct Instruction
{
    enum class Type
    {
        SPIN,
        EXCHANGE,
        PARTNER
    };
    Type t;
    int first;
    int second;

    Instruction(std::string_view sv)
    {
        if (sv.empty())
        {
            throw std::invalid_argument("Empty instruction");
        }
        switch (sv[0])
        {
        case 's':
            t = Type::SPIN;
            first = toNum(sv.substr(1));
            second = 0;
            break;
        case 'x':
        {
            t = Type::EXCHANGE;
            auto indices = readNumbers(sv);
            if (indices.size() != 2)
            {
                throw std::invalid_argument("Malformed input " + std::string(sv));
            }
            first = indices[0];
            second = indices[1];
            break;
        }
        case 'p':
        {
            t = Type::PARTNER;
            if (sv.size() != 4)
            {
                throw std::invalid_argument("Malformed input " + std::string(sv));
            }
            first = sv[1];
            second = sv[3];
            break;
        }
        default:
            throw std::invalid_argument("Malformed input " + std::string(sv));
        }
    }

    void apply(std::string& order) const
    {
        switch (t)
        {
        case Type::SPIN:
            std::ranges::rotate(order, std::next(order.begin(), std::ssize(order) - first));
            break;
        case Type::EXCHANGE:
            std::swap(order[first], order[second]);
            break;
        case Type::PARTNER:
        {
            auto fst = std::ranges::find(order, static_cast<char>(first));
            auto snd = std::ranges::find(order, static_cast<char>(second));
            std::swap(*fst, *snd);
            break;
        }
        }
    }
};
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    static constexpr auto PART2_REPEATS = 1'000'000'000UZ;
    auto instructions =
        trim(slurp(input)) | std::views::split(',') |
        std::views::transform([](auto sv)
                              { return Instruction{std::string_view{sv.begin(), sv.end()}}; }) |
        std::ranges::to<std::vector>();

    auto order = std::views::iota('a', static_cast<char>('a' + N)) | std::ranges::to<std::string>();
    auto orders = std::vector<std::string>{};
    do
    {
        orders.push_back(order);
        for (const auto& instr : instructions)
        {
            instr.apply(order);
        }
    } while (order != orders[0]);

    return {orders.size() > 1 ? orders[1] : orders[0], orders[PART2_REPEATS % orders.size()]};
}
} // namespace aoc
