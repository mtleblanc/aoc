#include "aoc.hh"

/* https://adventofcode.com/2017/day/9
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 9;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
auto removeGarbage(std::string_view dirty)
{
    std::string res;
    bool inGarbage{};
    bool skip{};
    int removed{};
    for (auto c : dirty)
    {
        if (!inGarbage)
        {
            if (c != '<')
            {
                res.push_back(c);
            }
            else
            {
                inGarbage = true;
            }
        }
        else
        {
            if (skip)
            {
                skip = false;
            }
            else if (c == '!')
            {
                skip = true;
            }
            else if (c == '>')
            {
                inGarbage = false;
            }
            else
            {
                ++removed;
            }
        }
    }
    return std::make_pair(res, removed);
}

int score(std::string_view nested)
{
    int res{};
    int depth{1};
    for (auto c : nested)
    {
        if (c == '{')
        {
            ++depth;
        }
        else if (c == '}')
        {
            res += --depth;
        }
    }
    return res;
}
} // namespace
template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto text = slurp(input);
    auto [cleaned, removed] = removeGarbage(text);

    return {score(cleaned), removed};
}
} // namespace aoc
