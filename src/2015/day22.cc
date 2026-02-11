#include "aoc.hh"
#include <iostream>
#include <queue>
#include <scn/scan.h>

/* https://adventofcode.com/2015/day/22
 */
namespace aoc
{
constexpr size_t YEAR = 2015;
constexpr size_t DAY = 22;

namespace
{

// Represents the stats immediately before or after casting a spell, specifically the state is after
// start of player's turn health degeneration and effects
// Seems silly to template DEGEN but it runs 10% faster than having a member, likely just because
// the structs are 12% smaller.  Switching the small stats like ticks to uint8 is slower on M2
using stat_t = short;
using resource_t = short;

constexpr resource_t PLAYER_HEALTH = 49;
constexpr resource_t PLAYER_MANA = 500;
constexpr resource_t RECHARGE_MANA = 101;
constexpr resource_t SHIELD_ARMOR = 7;

struct ManaCost
{
    static constexpr resource_t MAGIC_MISSILE = 53;
    static constexpr resource_t DRAIN = 73;
    static constexpr resource_t SHIELD = 113;
    static constexpr resource_t POISON = 173;
    static constexpr resource_t RECHARGE = 229;
};

struct Damage
{
    static constexpr resource_t MAGIC_MISSILE = 4;
    static constexpr resource_t DRAIN = 2;
    static constexpr resource_t POISON = 3;
};

struct Duration
{
    static constexpr stat_t SHIELD = 6;
    static constexpr stat_t POISON = 6;
    static constexpr stat_t RECHARGE = 5;
};

template <resource_t DEGEN> struct ActionPoint
{

    resource_t playerHealth{PLAYER_HEALTH};
    resource_t playerMana{PLAYER_MANA};
    resource_t bossHealth{};
    resource_t spentMana{};
    stat_t shieldTicks{};
    stat_t rechargeTicks{};
    stat_t poisonTicks{};
    stat_t bossDamage{};

    // std::string history{};

    std::weak_ordering operator<=>(const ActionPoint& o) const
    {
        return spentMana <=> o.spentMana;
    }

    [[nodiscard]] bool won() const
    {
        return bossHealth <= 0;
    }

    [[nodiscard]] bool lost() const
    {
        return playerHealth <= 0;
    }

    void forEachAction(auto fn) const
    {
        auto doIfAlive = [&fn](auto next)
        {
            next.advanceToNextDecision();

            if (!next.lost())
            {
                fn(next);
            }
        };
        doIfAlive(magicMissile());
        doIfAlive(drain());
        if (!shieldTicks)
        {
            doIfAlive(shield());
        }
        if (!rechargeTicks)
        {
            doIfAlive(recharge());
        }
        if (!poisonTicks)
        {
            doIfAlive(poison());
        }
    }

    void applyEffects()
    {
        if (poisonTicks)
        {
            --poisonTicks;
            bossHealth -= Damage::POISON;
        }
        if (rechargeTicks)
        {
            --rechargeTicks;
            playerMana += RECHARGE_MANA;
        }
        if (shieldTicks)
        {
            --shieldTicks;
        }
    }

    void advanceToNextDecision()
    {
        applyEffects();
        if (bossHealth <= 0)
        {
            return;
        }
        playerHealth -= (shieldTicks ? std::max(1, bossDamage - SHIELD_ARMOR) : bossDamage);
        applyEffects();
        playerHealth -= DEGEN;
    }

    bool spend(resource_t mana)
    {
        playerMana -= mana;
        spentMana += mana;
        if (playerMana < 0)
        {
            playerHealth = 0;
            return false;
        }
        return true;
    }

    [[nodiscard]] ActionPoint magicMissile() const
    {
        ActionPoint next = *this;
        if (next.spend(ManaCost::MAGIC_MISSILE))
        {
            next.bossHealth -= Damage::MAGIC_MISSILE;
        }

        // next.history.append("m");
        return next;
    }

    [[nodiscard]] ActionPoint drain() const
    {
        ActionPoint next = *this;
        if (next.spend(ManaCost::DRAIN))
        {
            next.bossHealth -= Damage::DRAIN;
            next.playerHealth += Damage::DRAIN;
        }
        // next.history.append("d");
        return next;
    }

    [[nodiscard]] ActionPoint shield() const
    {
        ActionPoint next = *this;
        if (next.spend(ManaCost::SHIELD))
        {
            next.shieldTicks = Duration::SHIELD;
        }
        // next.history.append("s");
        return next;
    }
    [[nodiscard]] ActionPoint recharge() const
    {
        ActionPoint next = *this;
        if (next.spend(ManaCost::RECHARGE))
        {
            next.rechargeTicks = Duration::RECHARGE;
        }
        // next.history.append("r");
        return next;
    }
    [[nodiscard]] ActionPoint poison() const
    {
        ActionPoint next = *this;
        if (next.spend(ManaCost::POISON))
        {
            next.poisonTicks = Duration::POISON;
        }
        // next.history.append("p");
        return next;
    }
};

template <resource_t DEGEN> ssize_t optimize(ActionPoint<DEGEN> g)
{
    g.playerHealth -= DEGEN;
    std::priority_queue<ActionPoint<DEGEN>, std::deque<ActionPoint<DEGEN>>, std::greater<>> dijk;
    dijk.push(g);

    while (dijk.size())
    {
        auto cur = dijk.top();
        dijk.pop();
        if (cur.won())
        {
            // std::cout << cur.history << std::endl;
            return cur.spentMana;
        }
        cur.forEachAction([&dijk](auto x) { dijk.push(x); });
    }
    return std::numeric_limits<ssize_t>::max();
}
} // namespace

template <> SsizeSolution solve<YEAR, DAY>(std::istream& input)
{
    auto [hp, damage] =
        scn::scan<resource_t, stat_t>(slurp(input), "Hit Points: {}\nDamage: {}\n")->values();

    ActionPoint<0> g0{.bossHealth = hp, .bossDamage = damage};
    ActionPoint<1> g1{.bossHealth = hp, .bossDamage = damage};

    return {optimize<0>(g0), optimize<1>(g1)};
}
} // namespace aoc
