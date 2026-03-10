#pragma once
#include <algorithm>
#include <ranges>

namespace aoc
{
[[maybe_unused]] constexpr auto DIRS9 =
    std::views::cartesian_product(std::views::iota(-1, 2), std::views::iota(-1, 2));

namespace
{
template <auto f> consteval auto filter()
{
    using T = std::ranges::range_value_t<decltype(DIRS9)>;
    constexpr auto SZ = std::ranges::count_if(DIRS9, f);
    auto res = std::array<T, SZ>{};
    std::ranges::copy_if(DIRS9, res.begin(), f);
    return res;
}
} // namespace

[[maybe_unused]] constexpr auto DIRS8 = filter<[](auto p)
                                               {
                                                   auto [x, y] = p;
                                                   return x != 0 || y != 0;
                                               }>();

[[maybe_unused]] constexpr auto DIRS4 = filter<[](auto p)
                                               {
                                                   auto [x, y] = p;
                                                   return (x == 0) != (y == 0);
                                               }>();

template <typename T, size_t N>
concept TupleLike = requires { requires std::tuple_size<T>::value == N; };

template <TupleLike<2> T, TupleLike<2> U> T& operator+=(T& l, U r)
{
    std::get<0>(l) += std::get<0>(r);
    std::get<1>(l) += std::get<1>(r);
    return l;
}

template <TupleLike<2> T, TupleLike<2> U> T operator+(T l, U r)
{
    return l += r;
}

template <TupleLike<2> T, TupleLike<2> U> T& operator-=(T& l, U r)
{
    std::get<0>(l) -= std::get<0>(r);
    std::get<1>(l) -= std::get<1>(r);
    return l;
}

template <TupleLike<2> T, TupleLike<2> U> T operator-(T l, U r)
{
    return l -= r;
}
} // namespace aoc
