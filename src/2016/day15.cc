#include "aoc.hh"
#include <cassert>
#include <ctre.hpp>
#include <numeric>

/* https://adventofcode.com/2016/day/15
 */
namespace aoc
{
constexpr int YEAR = 2016;
constexpr int DAY = 15;

namespace
{
struct Disc
{
    int mod;
    int start;
    int id;
};

[[maybe_unused]] std::istream& operator>>(std::istream& is, Disc& d)
{
    constexpr auto PAT = ctll::fixed_string(
        R"(Disc #(\d+) has (\d+) positions; at time=(\d+), it is at position (\d+).)");
    constexpr auto ID = 1;
    constexpr auto MOD = 2;
    constexpr auto START_TIME = 3;
    constexpr auto START_POS = 4;
    std::string line;
    std::getline(is, line);
    if (auto m = ctre::match<PAT>(line))
    {
        d.id = m.get<ID>().to_number();
        d.mod = m.get<MOD>().to_number();
        auto startTime = m.get<START_TIME>().to_number() % d.mod;
        auto startPos = m.get<START_POS>().to_number() % d.mod;
        d.start = (startPos + d.mod - startTime) % d.mod;
        return is;
    }
    is.setstate(std::ios_base::failbit);
    return is;
}

class ChineseRemainder
{
    std::vector<int> moduli;
    // M = \prod moduli
    int M{1};
    // characters[i] % moduli[j] = delta_ij
    std::vector<int> characters;

    static constexpr std::pair<int, int> euler(int m, int n)
    {
        if (m < n)
        {
            auto [y, x] = euler(n, m);
            return std::make_pair(x, y);
        }
        if (n == 0)
        {
            return std::make_pair(0, 1);
        }
        auto r = m % n;
        if (r == 0)
        {
            return std::make_pair(0, 1);
        }
        auto d = m / n;
        auto [x, y] = euler(n, r);
        return std::make_pair(y, x - d * y);
    }

  public:
    ChineseRemainder() = default;
    void addModulus(int modulus)
    {
        if (modulus < 0)
        {
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
        int character = static_cast<int>(int64_t{x} * M % (int64_t{M} * modulus));
        M *= modulus;
        for (auto& c : characters)
        {
            c += ((modulus - (c % modulus)) * character) % M;
        }
        moduli.push_back(modulus);
        characters.push_back(character);
    }

    int solve(const std::vector<int>& remainders)
    {
        if (remainders.size() != moduli.size())
        {
            throw std::invalid_argument("Remainders don't match moduli");
        }
        return std::inner_product(characters.begin(), characters.end(), remainders.begin(), 0) % M;
    }
};

} // namespace

template <> Solution_t<YEAR, DAY> solve<YEAR, DAY>(std::istream& input)
{
    constexpr auto PART2_DISC = 11;
    auto discs = readAll<Disc>(input);
    ChineseRemainder cr;
    std::vector<int> remainders;
    for (auto d : discs)
    {
        cr.addModulus(d.mod);
        remainders.push_back(d.mod - (d.id + d.start) % d.mod);
    }
    auto part1 = cr.solve(remainders);
    cr.addModulus(PART2_DISC);
    remainders.push_back(PART2_DISC - (static_cast<int>(remainders.size()) + 1) % PART2_DISC);
    return {part1, cr.solve(remainders)};
}
} // namespace aoc
