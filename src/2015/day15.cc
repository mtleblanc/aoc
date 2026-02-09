#include "aoc.hh"
#include <algorithm>
#include <cmath>
#include <numeric>

/* https://adventofcode.com/2015/day/15
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 15;

namespace
{
struct Ingredient
{
    long capacity{};
    long durability{};
    long flavour{};
    long texture{};
    long calories{};

    Ingredient& operator+=(const Ingredient& o)
    {
        capacity += o.capacity;
        durability += o.durability;
        flavour += o.flavour;
        texture += o.texture;
        calories += o.calories;
        return *this;
    }

    Ingredient& operator*=(long amount)
    {
        capacity *= amount;
        durability *= amount;
        flavour *= amount;
        texture *= amount;
        calories *= amount;
        return *this;
    }

    [[nodiscard]] long score() const
    {
        // NB no calories
        if (capacity < 0 || durability < 0 || flavour < 0 || texture < 0)
        {
            return 0;
        }
        return capacity * durability * flavour * texture;
    }
};

Ingredient operator+(Ingredient lhs, const Ingredient& rhs)
{
    return lhs += rhs;
}

Ingredient operator*(Ingredient lhs, const long rhs)
{
    return lhs *= rhs;
}

std::istream& operator>>(std::istream& is, Ingredient& i)
{
    // Sprinkles: capacity 2, durability 0, flavor -2, texture 0, calories 3
    std::string tmp;
    is >> tmp >> tmp >> i.capacity >> tmp >> tmp >> i.durability >> tmp >> tmp >> i.flavour >>
        tmp >> tmp >> i.texture >> tmp >> tmp >> i.calories;
    return is;
}

ssize_t score(const std::vector<Ingredient>& ingredients, const std::vector<long>& amounts)
{
    return std::inner_product(ingredients.begin(), ingredients.end(), amounts.begin(), Ingredient{})
        .score();
}

template <long CALORIES>
ssize_t scoreCalories(const std::vector<Ingredient>& ingredients, const std::vector<long>& amounts)
{
    auto amalgum =
        std::inner_product(ingredients.begin(), ingredients.end(), amounts.begin(), Ingredient{});
    return amalgum.calories == CALORIES ? amalgum.score() : 0;
}

// This is quite wasteful, it's just iterating through numbers in base CAPACITY and using the digits
// as amounts for the first d-1 ingredients, then putting the difference in the last ingredient
// Most of the time this will use more than CAPACITY for the first d-1, but the total iterations on
// the problem input are only 1 million, so it's still more than fast enough
template <long CAPACITY> std::vector<long> partitionFor(long n, ssize_t d)
{
    std::vector<long> amounts;
    for (ssize_t i = 0; i < d - 1; ++i)
    {
        amounts.push_back(n % CAPACITY);
        n /= CAPACITY;
    }
    amounts.push_back(CAPACITY - std::ranges::fold_left(amounts, 0, std::plus<>()));
    return amounts;
}

template <long CAPACITY, long CALORIES>
Solution optimize(const std::vector<Ingredient>& ingredients)
{
    ssize_t bestPart1{};
    ssize_t bestPart2{};
    ssize_t maxCount = std::pow(CAPACITY, ingredients.size() - 1);
    for (ssize_t i = 0; i < maxCount; ++i)
    {
        auto amounts = partitionFor<CAPACITY>(i, ingredients.size());
        if (amounts.back() < 0)
        {
            continue;
        }
        bestPart1 = std::max(bestPart1, score(ingredients, amounts));
        bestPart2 = std::max(bestPart2, scoreCalories<CALORIES>(ingredients, amounts));
    }
    return {bestPart1, bestPart2};
}

} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    constexpr long CAPACITY = 100;
    constexpr long CALORIES = 500;
    std::vector<Ingredient> ingredients;
    for (Ingredient i; input >> i;)
    {
        ingredients.push_back(i);
    }
    return optimize<CAPACITY, CALORIES>(ingredients);
}
} // namespace aoc
