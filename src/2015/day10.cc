#include "aoc.hh"

/* https://adventofcode.com/2015/day/10
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 10;

namespace
{
// Digit seems slightly faster than other types.  the sequence never has a digit over 3, but
// subword access is slow
using Digit = uint32_t;

void lookAndSay(const std::vector<Digit>& in, std::vector<Digit>& out)
{
    out.clear();
    if (in.empty())
    {
        return;
    }
    // grow while empty to avoid moves.  growth rate is 1.3 per iteration, buffers are swapping so
    // need to grow 1.3^2 = 1.7.  more precise than 2 shows no speedup
    out.reserve(in.size() * 2);
    Digit prev{in.front()};
    Digit seq{};
    for (auto x : in)
    {
        if (x == prev)
        {
            ++seq;
        }
        else
        {
            out.push_back(seq);
            out.push_back(prev);
            seq = 1;
            prev = x;
        }
    }
    out.push_back(seq);
    out.push_back(prev);
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    std::vector<Digit> sequence;
    for (char c{}; input >> c;)
    {
        sequence.push_back(c - '0');
    }
    std::vector<Digit> next;
    constexpr size_t ROUNDS_1 = 40;
    constexpr size_t ROUNDS_2 = 50;
    for (size_t i = 0; i < ROUNDS_1; i++)
    {
        lookAndSay(sequence, next);
        std::swap(next, sequence);
    }
    auto part1 = std::ssize(sequence);
    for (size_t i = ROUNDS_1; i < ROUNDS_2; i++)
    {
        lookAndSay(sequence, next);
        std::swap(next, sequence);
    }

    return {part1, std::ssize(sequence)};
}
} // namespace aoc
