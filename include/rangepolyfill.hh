#pragma once

#include <cstddef>
#include <ranges>
#include <tuple>

namespace aoc
{
template <size_t N, std::ranges::forward_range Rng>
struct AdjacentView : std::ranges::view_interface<AdjacentView<N, Rng>>
{

    class Iterator
    {
        friend class Sentinel;
        using underlying_iterator = std::ranges::iterator_t<Rng>;
        using underlying_value = typename std::ranges::range_value_t<Rng>;
        using underlying_reference = typename std::ranges::range_reference_t<Rng>;

        underlying_iterator it_;

        template <typename T, typename = std::make_index_sequence<N>> struct repeat_tuple;
        template <typename T, size_t... Is> struct repeat_tuple<T, std::index_sequence<Is...>>
        {
            template <size_t> using Ti = T;
            using type = std::tuple<Ti<Is>...>;
        };

      public:
        using difference_type = underlying_iterator::difference_type;
        using value_type = repeat_tuple<underlying_value>::type;
        using reference = repeat_tuple<underlying_reference>::type;
        using iterator_category = underlying_iterator::iterator_category;

        Iterator(underlying_iterator rng_it) : it_{rng_it} {}
        Iterator& operator++()
        {
            ++it_;
            return *this;
        }
        Iterator operator++(int)
        {
            Iterator retval = *this;
            ++(*this);
            return retval;
        }
        bool operator==(Iterator other) const
        {
            return it_ == other.it_;
        }
        bool operator!=(Iterator other) const
        {
            return !(*this == other);
        }
        reference operator*() const
        {
            return refHelper(std::make_index_sequence<N>{});
        }

      private:
        template <size_t... Ns> reference refHelper(std::index_sequence<Ns...> _) const
        {
            return {*std::next(it_, Ns)...};
        }
    };

    struct Sentinel
    {
        std::ranges::sentinel_t<Rng> end_;
        bool operator==(Iterator it) const
        {

            for (auto _ : std::views::iota(0UL, N))
            {
                if (it.it_ == end_)
                {
                    return true;
                }
                ++it.it_;
            }
            return false;
        }
    };

    template <std::ranges::forward_range Arg> AdjacentView(Arg&& arg) : rng_(std::forward<Arg>(arg))
    {
    }

    auto begin()
    {
        return Iterator{rng_.begin()};
    }
    auto end()
    {
        if constexpr (std::ranges::sized_range<Rng>)
        {
            auto sz = std::ranges::size(rng_);
            return sz < N ? begin() : Iterator{std::next(rng_.begin(), sz - N + 1)};
        }
        else
        {
            return Sentinel{rng_.end()};
        }
    }

  private:
    Rng rng_;
};

template <size_t N, std::ranges::forward_range Arg> auto adjacent(Arg&& arg)
{
    return AdjacentView<N, std::ranges::views::all_t<Arg>>{std::forward<Arg>(arg)};
}
} // namespace aoc