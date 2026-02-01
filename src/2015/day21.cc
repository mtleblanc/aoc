#include "aoc.hh"
#include <iostream>
#include <ranges>
#include <scn/scan.h>

/* https://adventofcode.com/2015/day/21

Weapons:    Cost  Damage  Armor
Dagger        8     4       0
Shortsword   10     5       0
Warhammer    25     6       0
Longsword    40     7       0
Greataxe     74     8       0

Armor:      Cost  Damage  Armor
Leather      13     0       1
Chainmail    31     0       2
Splintmail   53     0       3
Bandedmail   75     0       4
Platemail   102     0       5

Rings:      Cost  Damage  Armor
Damage +1    25     1       0
Damage +2    50     2       0
Damage +3   100     3       0
Defense +1   20     0       1
Defense +2   40     0       2
Defense +3   80     0       3

 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 21;

namespace
{
struct Item
{
    size_t cost;
    int damage;
    int armor;
    Item(size_t cost, int damage, int armor) : cost{cost}, damage{damage}, armor{armor} {}
};

const std::vector<Item> weapons = {{8, 4, 0}, {10, 5, 0}, {25, 6, 0}, {40, 7, 0}, {74, 8, 0}};
// include a 0 cost 0 stat item so armor is optional
const std::vector<Item> armors = {{13, 0, 1}, {31, 0, 2},  {53, 0, 3},
                                  {75, 0, 4}, {102, 0, 5}, {0, 0, 0}};
// and both rings are optional
const std::vector<Item> rings = {{25, 1, 0}, {50, 2, 0}, {100, 3, 0}, {20, 0, 1},
                                 {40, 0, 2}, {80, 0, 3}, {0, 0, 0},   {0, 0, 0}};

struct Stats
{
    int hp;
    int damage;
    int armor;
    size_t spent{};
    Stats(int hp, int damage, int armor) : hp{hp}, damage{damage}, armor{armor} {}

    class EquipmentGuard
    {
        Stats* user;
        Item item;

      public:
        EquipmentGuard(Stats& user, Item item) : user{&user}, item{item}
        {
            user.addStats(item);
        }
        EquipmentGuard(EquipmentGuard& o) = delete;
        EquipmentGuard(EquipmentGuard&& o) = default;
        EquipmentGuard& operator=(EquipmentGuard& o) = delete;
        EquipmentGuard& operator=(EquipmentGuard&& o) = default;
        ~EquipmentGuard()
        {
            user->removeStats(item);
        }
    };

    EquipmentGuard equip(Item item)
    {
        return {*this, item};
    }

  private:
    Stats& addStats(Item item)
    {
        damage += item.damage;
        armor += item.armor;
        spent += item.cost;
        return *this;
    }
    Stats& removeStats(Item item)
    {
        damage -= item.damage;
        armor -= item.armor;
        spent -= item.cost;
        return *this;
    }
};

int rounds(int damage, int hp)
{
    if (damage <= 0)
    {
        damage = 1;
    }
    return (hp + damage - 1) / damage;
}
bool playerWins(const Stats& player, const Stats& monster)
{
    return rounds(player.damage - monster.armor, monster.hp) <=
           rounds(monster.damage - player.armor, player.hp);
}

size_t part1(const Stats& monster)
{
    constexpr int PLAYER_HEALTH = 100;
    size_t minCost = std::numeric_limits<size_t>::max();
    Stats player{PLAYER_HEALTH, 0, 0};
    auto equip = [&player](auto item) { return player.equip(item); };

    auto weaponView = weapons | std::views::transform(equip);
    auto armorView = armors | std::views::transform(equip);
    auto ringView = rings | std::views::transform(equip);
    for (auto w : weaponView)
    {
        for (auto a : armorView)
        {
            // I don't have views::enumerate in my libc++
            // for (auto [i, r1]: ringView | std::views::enumerate)
            for (size_t first{}; first < rings.size(); ++first)
            {
                auto r1g = player.equip(rings[first]);
                for (auto r2 : ringView | std::views::drop(first))
                {
                    if (playerWins(player, monster))
                    {
                        minCost = std::min(minCost, player.spent);
                    }
                }
            }
        }
    }
    return minCost;
}

size_t part2(const Stats& monster)
{
    constexpr int PLAYER_HEALTH = 100;
    size_t maxCost = std::numeric_limits<size_t>::min();
    Stats player{PLAYER_HEALTH, 0, 0};
    auto equip = [&player](auto item) { return player.equip(item); };
    for (auto weapon : weapons | std::views::transform(equip))
    {
        for (auto armor : armors | std::views::transform(equip))
        {
            for (size_t first{}; first < rings.size(); ++first)
            {
                auto r1g = player.equip(rings[first]);
                for (size_t second{first + 1}; second < rings.size(); ++second)
                {
                    auto r2g = player.equip(rings[second]);
                    if (!playerWins(player, monster))
                    {
                        maxCost = std::max(maxCost, player.spent);
                    }
                }
            }
        }
    }
    return maxCost;
}

} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{

    auto [hp, damage, armor] =
        scn::scan<int, int, int>(slurp(input), "Hit Points: {}\nDamage: {}\nArmor: {}\n")->values();
    Stats monster{hp, damage, armor};
    return {part1(monster), part2(monster)};
}
} // namespace aoc
