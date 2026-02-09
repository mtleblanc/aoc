#include "aoc.hh"
#include <cassert>
#include <ctre.hpp>
#include <numeric>

/* https://adventofcode.com/2016/day/15
 */
namespace aoc
{
constexpr ssize_t YEAR = 2016;
constexpr ssize_t DAY = 15;

namespace
{
struct Disc
{
    ssize_t mod;
    ssize_t start;
    ssize_t id;
};

[[maybe_unused]] std::istream& operator>>(std::istream& is, Disc& d)
{
    constexpr auto PAT = ctll::fixed_string(
        R"(Disc #(\d+) has (\d+) positions; at time=(\d+), it is at position (\d+).)");
    constexpr auto ID = 1UL;
    constexpr auto MOD = 2UL;
    constexpr auto START_TIME = 3UL;
    constexpr auto START_POS = 4UL;
    std::string line;
    std::getline(is, line);
    if (auto m = ctre::match<PAT>(line))
    {
        d.id = std::stol(std::string(m.get<ID>()));
        d.mod = std::stol(std::string(m.get<MOD>()));
        auto startTime = std::stol(std::string(m.get<START_TIME>())) % d.mod;
        auto startPos = std::stol(std::string(m.get<START_POS>())) % d.mod;
        d.start = (startPos + d.mod - startTime) % d.mod;
        return is;
    }
    is.setstate(std::ios_base::failbit);
    return is;
}

class ChineseRemainder
{
    std::vector<ssize_t> moduli;
    // M = \prod moduli
    ssize_t M{1};
    // characters[i] % moduli[j] = delta_ij
    std::vector<ssize_t> characters;

    static constexpr std::pair<ssize_t, ssize_t> euler(ssize_t m, ssize_t n)
    {
        if (m < n)
        {
            auto [y, x] = euler(n, m);
            return std::make_pair(x, y);
        }
        if (n == 0)
        {
            return std::make_pair(0L, 1L);
        }
        auto r = m % n;
        if (r == 0)
        {
            return std::make_pair(0L, 1L);
        }
        auto d = m / n;
        auto [x, y] = euler(n, r);
        return std::make_pair(y, x - d * y);
    }

  public:
    ChineseRemainder() = default;
    void addModulus(ssize_t modulus)
    {
        if(modulus < 0) {
            addModulus(-modulus);
            return;
        }
        auto [x, y] = euler(M, modulus);
        if (x * M + y * modulus != 1)
        {
            throw std::invalid_argument("Modulus not coprime");
        }
        if (x < 0)
        {
            auto nextM = (M * modulus);
            x = x % nextM + nextM;
        }
        auto character = x * M % (M * modulus);
        M *= modulus;
        for (auto& c : characters)
        {
            c += ((modulus - (c % modulus)) * character) % M;
        }
        moduli.push_back(modulus);
        characters.push_back(character);
    }

    ssize_t solve(const std::vector<ssize_t>& remainders)
    {
        if (remainders.size() != moduli.size())
        {
            throw std::invalid_argument("Remainders don't match moduli");
        }
        return std::inner_product(characters.begin(), characters.end(), remainders.begin(), 0L) %
               M;
    }
};

} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    constexpr auto PART2_DISC = 11L;
    auto discs = readAll<Disc>(input);
    ChineseRemainder cr;
    std::vector<ssize_t> remainders;
    for (auto d : discs)
    {
        cr.addModulus(d.mod);
        remainders.push_back(d.mod - (d.id + d.start) % d.mod);
    }
    auto part1 = cr.solve(remainders);
    cr.addModulus(PART2_DISC);
    remainders.push_back(PART2_DISC - (std::ssize(remainders) + 1) % PART2_DISC);
    return {part1, cr.solve(remainders)};
}
} // namespace aoc
