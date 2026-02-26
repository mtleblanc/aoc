#include "aoc.hh"
#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <ranges>

/* https://adventofcode.com/2017/day/25
 */
namespace aoc
{
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 25;
using Solution = Solution_t<YEAR, DAY>;

namespace
{
// int is marginally fastest on my m2
using Value = int;

class Tape
{
    static constexpr auto SEG_SIZE = 1000;
    using Segment = std::array<Value, SEG_SIZE>;
    struct Node
    {
        Segment data{};
        Node* left{nullptr};
        Node* right{nullptr};
    };
    Node* cur{};
    int idx;
    std::vector<std::unique_ptr<Node>> nodes;

  public:
    Tape() : idx{SEG_SIZE / 2}
    {
        nodes.push_back(std::make_unique<Node>());
        cur = nodes.front().get();
    }

    Value& operator*()
    {
        return cur->data[idx];
    }

    void left()
    {
        if (--idx < 0)
        {
            idx = SEG_SIZE - 1;
            if (cur->left == nullptr)
            {
                nodes.push_back(std::make_unique<Node>());
                cur->left = nodes.back().get();
                cur->left->right = cur;
            }
            cur = cur->left;
        }
    }

    void right()
    {
        if (++idx == SEG_SIZE)
        {
            idx = 0;
            if (cur->right == nullptr)
            {
                nodes.push_back(std::make_unique<Node>());
                cur->right = nodes.back().get();
                cur->right->left = cur;
            }
            cur = cur->right;
        }
    }

    int checksum()
    {
        using std::ranges::fold_left;
        using std::views::transform;
        auto sumNode = [](auto& s) { return fold_left(s->data, 0, std::plus<>()); };
        return fold_left(nodes | transform(sumNode), 0, std::plus<>());
    }
};

struct State;

struct Transition
{
    enum class Direction
    {
        LEFT,
        RIGHT
    };

    Value write{};
    Direction dir{};
    State* next{};
};

struct State
{
    Transition onOne;
    Transition onZero;
};

class TuringMachine
{
    std::map<char, State> states;
    State* state{};
    Tape tape;

  public:
    TuringMachine()
    {
        // TODO: Read from input
        using Transition::Direction::LEFT;
        using Transition::Direction::RIGHT;
        auto* s = &states['A'];
        auto* t = &s->onZero;
        t->write = 1;
        t->dir = RIGHT;
        t->next = &states['B'];
        t = &s->onOne;
        t->write = 0;
        t->dir = LEFT;
        t->next = &states['C'];

        s = &states['B'];
        t = &s->onZero;
        t->write = 1;
        t->dir = LEFT;
        t->next = &states['A'];
        t = &s->onOne;
        t->write = 1;
        t->dir = RIGHT;
        t->next = &states['C'];

        s = &states['C'];
        t = &s->onZero;
        t->write = 1;
        t->dir = RIGHT;
        t->next = &states['A'];
        t = &s->onOne;
        t->write = 0;
        t->dir = LEFT;
        t->next = &states['D'];

        s = &states['D'];
        t = &s->onZero;
        t->write = 1;
        t->dir = LEFT;
        t->next = &states['E'];
        t = &s->onOne;
        t->write = 1;
        t->dir = LEFT;
        t->next = &states['C'];

        s = &states['E'];
        t = &s->onZero;
        t->write = 1;
        t->dir = RIGHT;
        t->next = &states['F'];
        t = &s->onOne;
        t->write = 1;
        t->dir = RIGHT;
        t->next = &states['A'];

        s = &states['F'];
        t = &s->onZero;
        t->write = 1;
        t->dir = RIGHT;
        t->next = &states['A'];
        t = &s->onOne;
        t->write = 1;
        t->dir = RIGHT;
        t->next = &states['E'];

        state = &states['A'];
    }

    int checksum()
    {
        return tape.checksum();
    }

    void run(int steps)
    {
        while (steps-- > 0)
        {
            auto* t = *tape == 1 ? &state->onOne : &state->onZero;
            *tape = t->write;
            if (t->dir == Transition::Direction::LEFT)
            {
                tape.left();
            }
            else
            {
                tape.right();
            }
            state = t->next;
        }
    }
};
} // namespace

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    (void)input;
    auto tm = TuringMachine{};
    tm.run(12261543);
    return {tm.checksum()};
}
} // namespace aoc
