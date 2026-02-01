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
struct ActionPoint
{
    // NOLINTBEGIN (performance-enum-size)
    enum class ManaCost : int
    {
        MAGIC_MISSILE = 53,
        DRAIN = 73,
        SHIELD = 113,
        POISON = 173,
        RECHARGE = 229
    };
    enum class Damage : int
    {
        MAGIC_MISSILE = 4,
        DRAIN = 2,
        POISON = 3
    };
    enum class Duration : int
    {
        SHIELD = 6,
        POISON = 6,
        RECHARGE = 5
    };
    // NOLINTEND
    static constexpr int PLAYER_HEALTH = 49;
    static constexpr int PLAYER_MANA = 500;
    static constexpr int RECHARGE_MANA = 101;
    static constexpr int SHIELD_ARMOR = 7;

    int playerHealth{PLAYER_HEALTH};
    int playerMana{PLAYER_MANA};
    int shieldTicks{};
    int rechargeTicks{};
    int poisonTicks{};
    int spentMana{};
    int bossHealth{};
    int bossDamage{};
    int degen{};

    // std::string history{};

    std::weak_ordering operator<=>(const ActionPoint& o) const
    {
        return spentMana <=> o.spentMana;
    }

    [[nodiscard]] bool won() const
    {
        return bossHealth == 0;
    }

    [[nodiscard]] bool lost() const
    {
        return playerHealth == 0;
    }

    [[nodiscard]] auto actions() const
    {
        auto acts = std::vector{&ActionPoint::magicMissile, &ActionPoint::drain};
        if (!shieldTicks)
        {
            acts.push_back(&ActionPoint::shield);
        }
        if (!rechargeTicks)
        {
            acts.push_back(&ActionPoint::recharge);
        }
        if (!poisonTicks)
        {
            acts.push_back(&ActionPoint::poison);
        }
        return acts;
    }

    /*
     * advance from the point where a player has just cast a spell to just before they cast their
     * next spell
     *
     * i.e. Player's turn
     *.     helth degenerates
     *      effects tick
     *      player casts spell
     *      ----- we are here
     *      Boss's turn
     *      effects tick
     *      boss attacks
     *      Player's turn
     *      helth degenerates
     *      effects tick
     *      ---- get to here
     *
     * We need to split poison ticks in case the first tick kills boss before attack, or boss's
     * attack kills player before second tick. The other effects can just be done 2x at the end
     */
    void advanceToNextDecision()
    {
        bossHealth -= poisonTicks ? static_cast<int>(Damage::POISON) : 0;
        poisonTicks = std::max(0, poisonTicks - 1);
        if (bossHealth <= 0)
        {
            bossHealth = 0;
            return;
        }

        // shield ticks reduce by 1 before boss attack, but don't update until we combine it with
        // the tick before the player's next turn
        playerHealth -= (shieldTicks > 1 ? std::max(1, bossDamage - SHIELD_ARMOR) : bossDamage);
        playerHealth -= degen;
        if (playerHealth <= 0)
        {
            playerHealth = 0;
            return;
        }
        shieldTicks = std::max(0, shieldTicks - 2);
        bossHealth -= poisonTicks ? static_cast<int>(Damage::POISON) : 0;
        poisonTicks = std::max(0, poisonTicks - 1);
        playerMana += std::min(rechargeTicks, 2) * RECHARGE_MANA;
        rechargeTicks = std::max(0, rechargeTicks - 2);
    }

    bool spend(ManaCost mana)
    {
        playerMana -= static_cast<int>(mana);
        spentMana += static_cast<int>(mana);
        if (playerMana < 0)
        {
            playerHealth = 0;
            return false;
        }
        return true;
    }

    ActionPoint magicMissile()
    {
        ActionPoint next = *this;
        if (next.spend(ManaCost::MAGIC_MISSILE))
        {
            next.bossHealth -= static_cast<int>(Damage::MAGIC_MISSILE);
        }

        // next.history.append("m");
        return next;
    }

    ActionPoint drain()
    {
        ActionPoint next = *this;
        if (next.spend(ManaCost::DRAIN))
        {
            next.bossHealth -= static_cast<int>(Damage::DRAIN);
            next.playerHealth += static_cast<int>(Damage::DRAIN);
        }
        // next.history.append("d");
        return next;
    }

    ActionPoint shield()
    {
        ActionPoint next = *this;
        if (next.spend(ManaCost::SHIELD))
        {
            next.shieldTicks = static_cast<int>(Duration::SHIELD);
        }
        // next.history.append("s");
        return next;
    }
    ActionPoint recharge()
    {
        ActionPoint next = *this;
        if (next.spend(ManaCost::RECHARGE))
        {
            next.rechargeTicks = static_cast<int>(Duration::RECHARGE);
        }
        // next.history.append("r");
        return next;
    }
    ActionPoint poison()
    {
        ActionPoint next = *this;
        if (next.spend(ManaCost::POISON))
        {
            next.poisonTicks = static_cast<int>(Duration::POISON);
        }
        // next.history.append("p");
        return next;
    }
};

size_t optimize(ActionPoint g, int degen)
{
    g.degen = degen;
    g.playerHealth -= degen;
    std::priority_queue<ActionPoint, std::deque<ActionPoint>, std::greater<>> dijk;
    dijk.push(g);

    for (;;)
    {
        auto cur = dijk.top();
        dijk.pop();
        if (cur.won())
        {
            // std::cout << cur.history << std::endl;
            return cur.spentMana;
        }

        for (auto a : cur.actions())
        {
            auto next = std::invoke(a, cur);
            next.advanceToNextDecision();
            if (!next.lost())
            {
                dijk.push(next);
            }
        }
    }
}
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    auto [hp, damage] = scn::scan<int, int>(slurp(input), "Hit Points: {}\nDamage: {}\n")->values();

    ActionPoint g{.bossHealth = hp, .bossDamage = damage};

    return {optimize(g, 0), optimize(g, 1)};
}
} // namespace aoc
