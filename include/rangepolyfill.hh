#pragma once

#include <cstddef>
#include <optional>
#include <ranges>
#include <tuple>

namespace aoc
{

template <typename T>
concept CommonAdaptable = std::ranges::common_range<T> &&
                          (std::ranges::bidirectional_range<T> || std::ranges::sized_range<T>);

template <size_t N, std::ranges::forward_range Rng>
struct AdjacentView : std::ranges::view_interface<AdjacentView<N, Rng>>
{

    struct Sentinel;

    class Iterator
    {
        friend struct Sentinel;
        using It = std::ranges::iterator_t<Rng>;
        using Val = std::ranges::range_value_t<Rng>;
        using Ref = std::ranges::range_reference_t<Rng>;

        It it_;
        It fit_;

        template <typename T, typename = std::make_index_sequence<N>> struct repeat_tuple;
        template <typename T, size_t... Is> struct repeat_tuple<T, std::index_sequence<Is...>>
        {
            template <size_t> using Ti = T;
            using type = std::tuple<Ti<Is>...>;
        };

      public:
        using difference_type = std::ranges::range_difference_t<Rng>;
        using value_type = repeat_tuple<Val>::type;
        using reference = repeat_tuple<Ref>::type;

        Iterator(It it, It fit) : it_{std::move(it)}, fit_{std::move(fit)} {}
        Iterator(It it = It{}) : it_{it}, fit_{std::move(it)} {}

        reference operator*() const
        {
            return refHelper(std::make_index_sequence<N>{});
        }
        Iterator& operator++()
        {
            ++it_;
            ++fit_;
            return *this;
        }
        Iterator operator++(int)
            requires(std::ranges::forward_range<Rng>)
        {
            Iterator retval = *this;
            ++(*this);
            return retval;
        }
        void operator++(int)
            requires(!std::ranges::forward_range<Rng>)
        {
            ++(*this);
        }
        bool operator==(const Iterator& other) const
        {
            return it_ == other.it_;
        }
        Iterator& operator--()
            requires(std::bidirectional_iterator<It>)
        {
            --it_;
            --fit_;
            return *this;
        }
        Iterator operator--(int)
            requires(std::bidirectional_iterator<It>)
        {
            Iterator retval = *this;
            --(*this);
            return retval;
        }
        Iterator& operator+=(difference_type n)
            requires(std::random_access_iterator<It>)
        {
            it_ += n;
            fit_ += n;
            return *this;
        }
        Iterator operator+(difference_type n) const
            requires(std::random_access_iterator<It>)
        {
            auto ret = *this;
            return ret += n;
        }
        Iterator& operator-=(difference_type n)
            requires(std::random_access_iterator<It>)
        {
            it_ -= n;
            fit_ -= n;
            return *this;
        }
        Iterator operator-(difference_type n) const
            requires(std::random_access_iterator<It>)
        {
            auto ret = *this;
            return ret -= n;
        }
        reference operator[](difference_type n) const
            requires(std::random_access_iterator<It>)
        {
            return *(*this + n);
        }
        auto operator<=>(const Iterator& o) const
            requires(std::random_access_iterator<It>)
        {
            return it_ <=> o.it_;
        }
        difference_type operator-(const Iterator& o) const
            requires(std::random_access_iterator<It>)
        {
            return it_ - o.it_;
        }
        friend Iterator operator+(difference_type n, const Iterator& o)
            requires(std::random_access_iterator<It>)
        {
            return o + n;
        }

      private:
        template <size_t... Ns>
        reference refHelper([[maybe_unused]] std::index_sequence<Ns...> _) const
        {
            return {*std::next(it_, Ns)...};
        }
    };

    struct Sentinel
    {
        std::optional<std::ranges::sentinel_t<Rng>> end_;

        bool operator==(const Iterator& it) const
        {
            return !end_ || it.fit_ == *end_;
        }
        std::ranges::range_difference_t<Rng> operator-(const Iterator& it) const
            requires(std::ranges::sized_range<Rng>)
        {
            return end_ ? *end_ - it.fit_ : 0;
        }
        friend std::ranges::range_difference_t<Rng> operator-(const Iterator& it, const Sentinel& s)
            requires(std::ranges::sized_range<Rng>)
        {
            return s.end_ ? *s.end_ - it.fit_ : 0;
        }
    };

    template <std::ranges::forward_range Arg> AdjacentView(Arg&& arg) : rng_(std::forward<Arg>(arg))
    {
        if constexpr (std::ranges::sized_range<Rng>)
        {
            empty = std::ranges::size(rng_) < N;
        }
        else
        {
            auto it = std::ranges::begin(rng_);
            auto end = std::ranges::end(rng_);
            for (auto _ : std::views::iota(0UL, N))
            {
                if (it == end)
                {
                    empty = true;
                    return;
                }
                ++it;
            }
        }
    }

    auto begin()
    {
        auto begin = std::ranges::begin(rng_);
        return Iterator{begin, empty ? begin : std::next(begin, N - 1)};
    }
    auto end()
        requires(!CommonAdaptable<Rng>)
    {
        if (empty)
        {
            return Sentinel{};
        }
        return Sentinel{std::ranges::end(rng_)};
    }
    auto end()
        requires(CommonAdaptable<Rng>)
    {
        if (empty)
        {
            return begin();
        }
        if constexpr (std::ranges::bidirectional_range<Rng>)
        {
            auto end = std::ranges::end(rng_);
            return Iterator{
                std::next(end, 1 - static_cast<std::ranges::range_difference_t<Rng>>(N))};
        }
        else if constexpr (std::ranges::sized_range<Rng>)
        {
            return std::next(begin(), N - 1);
        }
    }

  private:
    Rng rng_;
    bool empty{false};
};

template <size_t N, std::ranges::forward_range Arg> auto adjacent(Arg&& arg)
{
    return AdjacentView<N, std::ranges::views::all_t<Arg>>{std::forward<Arg>(arg)};
}

template <std::ranges::input_range Rng>
struct EnumerateView : std::ranges::view_interface<EnumerateView<Rng>>
{
    struct Sentinel;
    struct Iterator
    {
      private:
        friend struct Sentinel;
        using It = std::ranges::iterator_t<Rng>;
        using Val = std::ranges::range_value_t<Rng>;
        using Ref = std::ranges::range_reference_t<Rng>;

        It it_;
        size_t idx_{};

      public:
        using difference_type = std::ranges::range_difference_t<Rng>;
        using value_type = std::pair<size_t, Val>;
        using reference = std::pair<size_t, Ref>;

        Iterator()
            requires std::default_initializable<It>
        = default;
        Iterator(It it) : it_{std::move(it)} {};
        reference operator*() const
        {
            return {idx_, *it_};
        }
        Iterator& operator++()
        {
            ++it_;
            ++idx_;
            return *this;
        }
        Iterator operator++(int)
        {
            Iterator retval = *this;
            ++(*this);
            return retval;
        }
        bool operator==(const Iterator& other) const
            requires(std::ranges::forward_range<Rng>)
        {
            return it_ == other.it_;
        }
        Iterator& operator--()
            requires(std::bidirectional_iterator<It>)
        {
            --it_;
            --idx_;
            return *this;
        }
        Iterator operator--(int)
            requires(std::bidirectional_iterator<It>)
        {
            Iterator retval = *this;
            --(*this);
            return retval;
        }
        Iterator& operator+=(difference_type n)
            requires(std::random_access_iterator<It>)
        {
            it_ += n;
            idx_ += n;
            return *this;
        }
        Iterator operator+(difference_type n) const
            requires(std::random_access_iterator<It>)
        {
            auto ret = *this;
            return ret += n;
        }
        Iterator& operator-=(difference_type n)
            requires(std::random_access_iterator<It>)
        {
            it_ -= n;
            idx_ -= n;
            return *this;
        }
        Iterator operator-(difference_type n) const
            requires(std::random_access_iterator<It>)
        {
            auto ret = *this;
            return ret -= n;
        }
        reference operator[](difference_type n) const
            requires(std::random_access_iterator<It>)
        {
            return *(*this + n);
        }
        auto operator<=>(const Iterator& o) const
            requires(std::random_access_iterator<It>)
        {
            return it_ <=> o.it_;
        }
        difference_type operator-(const Iterator& o) const
            requires(std::random_access_iterator<It>)
        {
            return it_ - o.it_;
        }
        friend Iterator operator+(difference_type n, const Iterator& o)
            requires(std::random_access_iterator<It>)
        {
            return o + n;
        }
    };

    struct Sentinel
    {
        std::ranges::sentinel_t<Rng> end_;
        bool operator==(const Iterator& it) const
        {
            return it.it_ == end_;
        }
    };

    template <std::ranges::range Arg> EnumerateView(Arg&& arg) : rng_(std::forward<Arg>(arg)) {}

    auto begin()
    {
        return Iterator{std::ranges::begin(rng_)};
    }
    auto end()
    {
        return Sentinel{std::ranges::end(rng_)};
    }

    std::ranges::range_difference_t<Rng> size() const
        requires(std::ranges::sized_range<Rng>)
    {
        return rng_.size();
    }

  private:
    Rng rng_;
};

template <std::ranges::input_range Arg> auto enumerate(Arg&& arg)
{
    return EnumerateView<std::ranges::views::all_t<Arg>>{std::forward<Arg>(arg)};
}

} // namespace aoc
