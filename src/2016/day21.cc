#include "aoc.hh"
#include "util.hh"
#include <algorithm>
#include <ctre.hpp>
#include <memory>
#include <ranges>

/* https://adventofcode.com/2016/day/21
 */
namespace aoc
{
constexpr size_t YEAR = 2016;
constexpr size_t DAY = 21;

namespace
{
struct Instr
{
    Instr() = default;
    Instr(const Instr&) = delete;
    Instr(Instr&&) = delete;
    Instr& operator=(const Instr&) = delete;
    Instr& operator=(Instr&&) = delete;
    virtual void apply(std::string& str) const = 0;
    virtual void unapply(std::string& str) const = 0;
    virtual ~Instr() = default;
};

struct SwapPosition : Instr
{
    int first;
    int second;
    SwapPosition(int first, int second) : first{first}, second{second} {}
    void apply(std::string& str) const override
    {
        std::swap(str[first], str[second]);
    }
    void unapply(std::string& str) const override
    {
        apply(str);
    }
};

struct SwapLetters : Instr
{
    char first;
    char second;
    SwapLetters(char first, char second) : first{first}, second{second} {}
    void apply(std::string& str) const override
    {
        for (auto& c : str)
        {
            if (c == first)
            {
                c = second;
            }
            else if (c == second)
            {
                c = first;
            }
        }
    }
    void unapply(std::string& str) const override
    {
        apply(str);
    }
};

struct Rotate : Instr
{
    int amount;
    Rotate(int amount) : amount{amount} {}
    void apply(std::string& str) const override
    {
        auto start = mod(amount, std::ssize(str));
        std::ranges::rotate(str, std::next(str.begin(), start));
    }
    void unapply(std::string& str) const override
    {
        auto start = mod(-amount, std::ssize(str));
        std::ranges::rotate(str, std::next(str.begin(), start));
    }
};

struct RotateOnLetter : Instr
{
    char letter;
    RotateOnLetter(char letter) : letter{letter} {}
    void apply(std::string& str) const override
    {
        auto ind = std::distance(str.begin(), std::ranges::find(str, letter));
        auto start = mod(-(ind + 1 + (ind >= 4 ? 1 : 0)), std::ssize(str));
        std::ranges::rotate(str, std::next(str.begin(), start));
    }
    void unapply(std::string& str) const override
    {
        // if str doesn't have 8 letters, apply isn't injective
        // however it would be if str had an even number of letters and the extra rotation was for
        // indices in the second half
        //
        // this reverses that generalized transform, which works out to moving the index letter from
        // i -> 2i + 1 or 2i + 2 (mod len)
        // specifically, the first half ends up at odd indices without wrapping, the second half
        // at even indices wrapping once.  So to reverse, we need to account for the wrapping by
        // adding it to even indices (and also moving 0 to len), then rotate left by i+1 or i+2
        // both of which are floor(index/2) + 1
        auto len = std::ssize(str);
        auto ind = std::distance(str.begin(), std::ranges::find(str, letter));
        if (ind == 0)
        {
            ind = len;
        }
        if (ind % 2 == 0)
        {
            ind += len;
        }
        auto start = (ind / 2 + 1) % len;
        std::ranges::rotate(str, std::next(str.begin(), start));
    }
};

struct Reverse : Instr
{
    int first;
    int second;
    Reverse(int first, int second) : first{first}, second{second} {}
    void apply(std::string& str) const override
    {
        auto sv =
            std::views::all(str) | std::views::drop(first) | std::views::take(second - first + 1);
        std::ranges::reverse(sv);
    }
    void unapply(std::string& str) const override
    {
        apply(str);
    }
};

struct Move : Instr
{
    int from;
    int to;
    Move(int from, int to) : from{from}, to{to} {}
    void apply(std::string& str) const override
    {
        auto c = str[from];
        str.erase(from, 1);
        str.insert(std::next(str.begin(), to), c);
    }
    void unapply(std::string& str) const override
    {
        auto c = str[to];
        str.erase(to, 1);
        str.insert(std::next(str.begin(), from), c);
    }
};

std::unique_ptr<Instr> parse(const std::string& str)
{
    constexpr auto SWAP_POS = ctll::fixed_string(R"(swap position (\d+) with position (\d+))");
    constexpr auto SWAP_LETTER = ctll::fixed_string(R"(swap letter (\w) with letter (\w))");
    constexpr auto ROTATE = ctll::fixed_string(R"(rotate (left|right) (\d+) steps?)");
    constexpr auto ROTATE_LETTER = ctll::fixed_string(R"(rotate based on position of letter (\w))");
    constexpr auto REVERSE = ctll::fixed_string(R"(reverse positions (\d+) through (\d+))");
    constexpr auto MOVE = ctll::fixed_string(R"(move position (\d+) to position (\d+))");

    if (auto m = ctre::match<SWAP_POS>(str))
    {
        return std::make_unique<SwapPosition>(m.get<1>().to_number(), m.get<2>().to_number());
    }
    if (auto m = ctre::match<SWAP_LETTER>(str))
    {
        return std::make_unique<SwapLetters>(m.get<1>().to_view()[0], m.get<2>().to_view()[0]);
    }
    if (auto m = ctre::match<ROTATE>(str))
    {
        return std::make_unique<Rotate>(m.get<1>() == "left" ? m.get<2>().to_number()
                                                             : -m.get<2>().to_number());
    }
    if (auto m = ctre::match<ROTATE_LETTER>(str))
    {
        return std::make_unique<RotateOnLetter>(m.get<1>().to_view()[0]);
    }
    if (auto m = ctre::match<REVERSE>(str))
    {
        return std::make_unique<Reverse>(m.get<1>().to_number(), m.get<2>().to_number());
    }
    if (auto m = ctre::match<MOVE>(str))
    {
        return std::make_unique<Move>(m.get<1>().to_number(), m.get<2>().to_number());
    }
    throw std::invalid_argument("Unknown instruction");
}

} // namespace
template <> struct SolutionType<YEAR, DAY>
{
    using type = StringSolution;
};

template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    auto instructions =
        readAllLines(input) | std::views::transform(parse) | std::ranges::to<std::vector>();
    std::string pass{"abcdefgh"};
    for (auto& instr : instructions)
    {
        instr->apply(pass);
    }
    std::string reverse{"fbgdceah"};
    for (auto& instr : instructions | std::views::reverse)
    {
        instr->unapply(reverse);
    }
    return {pass, reverse};
}
} // namespace aoc
