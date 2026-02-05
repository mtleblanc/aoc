// NOLINTBEGIN
#include "rangepolyfill.hh"
#include <catch2/catch_test_macros.hpp>

#include <forward_list>
#include <iterator>
#include <list>
#include <string>
#include <vector>

using namespace aoc;

TEST_CASE("enumerate over vector of ints")
{
    std::vector<int> v = {10, 20, 30};
    std::vector<std::pair<size_t, int>> result;
    for (auto [i, val] : enumerate(v))
    {
        result.emplace_back(i, val);
    }
    REQUIRE(result.size() == 3);
    REQUIRE(result[0] == std::pair{0UL, 10});
    REQUIRE(result[1] == std::pair{1UL, 20});
    REQUIRE(result[2] == std::pair{2UL, 30});
}

TEST_CASE("enumerate over string")
{
    std::string s = "abc";
    std::vector<std::pair<size_t, char>> result;
    for (auto [i, ch] : enumerate(s))
    {
        result.emplace_back(i, ch);
    }
    REQUIRE(result.size() == 3);
    REQUIRE(result[0] == std::pair{0UL, 'a'});
    REQUIRE(result[1] == std::pair{1UL, 'b'});
    REQUIRE(result[2] == std::pair{2UL, 'c'});
}

TEST_CASE("enumerate over empty range")
{
    std::vector<int> v;
    size_t count = 0;
    for ([[maybe_unused]] auto tup : enumerate(v))
    {
        ++count;
    }
    REQUIRE(count == 0);
}

TEST_CASE("enumerate over single element")
{
    std::vector<int> v = {42};
    std::vector<std::pair<size_t, int>> result;
    for (auto [i, val] : enumerate(v))
    {
        result.emplace_back(i, val);
    }
    REQUIRE(result.size() == 1);
    REQUIRE(result[0] == std::pair{0UL, 42});
}

TEST_CASE("enumerate over view with sentinel")
{
    std::vector<int> v = {42, 43, 44, 45};
    auto w = std::views::take_while(v, [](auto n) { return n < 44; });
    std::vector<std::pair<size_t, int>> result;
    for (auto [i, val] : enumerate(w))
    {
        result.emplace_back(i, val);
    }
    REQUIRE(result.size() == 2);
    REQUIRE(result[0] == std::pair{0UL, 42});
    REQUIRE(result[1] == std::pair{1UL, 43});
}

TEST_CASE("enumerate over rvalue view")
{
    std::vector<int> v = {42, 43, 44, 45};
    std::vector<std::pair<size_t, int>> result;
    for (auto [i, val] : enumerate(std::views::take_while(v, [](auto n) { return n < 44; })))
    {
        result.emplace_back(i, val);
    }
    REQUIRE(result.size() == 2);
    REQUIRE(result[0] == std::pair{0UL, 42});
    REQUIRE(result[1] == std::pair{1UL, 43});
}

TEST_CASE("enumerate yields references", "[reference]")
{
    std::vector<int> v = {1, 2, 3};
    for (auto [i, val] : enumerate(v))
    {
        REQUIRE(&val == &v[i]);
    }
}

TEST_CASE("enumerate allows mutation through reference", "[reference]")
{
    std::vector<int> v = {1, 2, 3};
    for (auto [i, val] : enumerate(v))
    {
        val *= 10;
    }
    REQUIRE(v == std::vector{10, 20, 30});
}

TEST_CASE("enumerate over forward_list", "[sentinel]")
{
    std::forward_list<int> fl = {100, 200, 300};
    std::vector<std::pair<size_t, int>> result;
    for (auto [i, val] : enumerate(fl))
    {
        result.emplace_back(i, val);
    }
    REQUIRE(result.size() == 3);
    REQUIRE(result[0] == std::pair{0UL, 100});
    REQUIRE(result[1] == std::pair{1UL, 200});
    REQUIRE(result[2] == std::pair{2UL, 300});
}

TEST_CASE("enumerate over empty forward_list", "[sentinel]")
{
    std::forward_list<int> fl;
    size_t count = 0;
    for ([[maybe_unused]] auto tup : enumerate(fl))
    {
        ++count;
    }
    REQUIRE(count == 0);
}

TEST_CASE("enumerate post-increment")
{
    std::vector<int> v = {10, 20, 30};
    auto view = enumerate(v);
    auto it = view.begin();
    auto prev = it++;
    auto [i0, v0] = *prev;
    auto [i1, v1] = *it;
    REQUIRE(i0 == 0);
    REQUIRE(v0 == 10);
    REQUIRE(i1 == 1);
    REQUIRE(v1 == 20);
}

TEST_CASE("enumerate index type is size_t")
{
    std::vector<int> v = {1};
    for (auto [i, val] : enumerate(v))
    {
        STATIC_REQUIRE(std::is_same_v<decltype(i), size_t>);
        (void)val;
    }
}

TEST_CASE("enumerate satisfies input_iterator", "[category]")
{
    using view_t = decltype(enumerate(std::declval<std::ranges::istream_view<char>&>()));
    using iter_t = decltype(std::declval<view_t>().begin());
    STATIC_REQUIRE(std::input_iterator<iter_t>);
    STATIC_REQUIRE(!std::forward_iterator<iter_t>);
    STATIC_REQUIRE(std::ranges::input_range<view_t>);
    STATIC_REQUIRE(!std::ranges::forward_range<view_t>);
}

TEST_CASE("enumerate satisfies forward_iterator for forward range", "[category]")
{
    using view_t = decltype(enumerate(std::declval<std::forward_list<int>&>()));
    using iter_t = decltype(std::declval<view_t>().begin());
    STATIC_REQUIRE(std::forward_iterator<iter_t>);
    STATIC_REQUIRE(!std::bidirectional_iterator<iter_t>);
    STATIC_REQUIRE(std::ranges::forward_range<view_t>);
    STATIC_REQUIRE(!std::ranges::bidirectional_range<view_t>);
}

TEST_CASE("enumerate satisfies bidirectional_iterator for bidirectional range", "[category]")
{
    using view_t = decltype(enumerate(std::declval<std::list<int>&>()));
    using iter_t = decltype(std::declval<view_t>().begin());
    STATIC_REQUIRE(std::bidirectional_iterator<iter_t>);
    STATIC_REQUIRE(!std::random_access_iterator<iter_t>);
    STATIC_REQUIRE(std::ranges::bidirectional_range<view_t>);
    STATIC_REQUIRE(!std::ranges::random_access_range<view_t>);
}

TEST_CASE("enumerate satisfies random_access_iterator for random access range", "[category]")
{
    using view_t = decltype(enumerate(std::declval<std::vector<int>&>()));
    using iter_t = decltype(std::declval<view_t>().begin());
    STATIC_REQUIRE(std::random_access_iterator<iter_t>);
    STATIC_REQUIRE(std::ranges::random_access_range<view_t>);
}

TEST_CASE("enumerate not contiguous even for contiguous range", "[category]")
{
    using view_t = decltype(enumerate(std::declval<std::vector<int>&>()));
    using iter_t = decltype(std::declval<view_t>().begin());
    STATIC_REQUIRE(std::random_access_iterator<iter_t>);
    STATIC_REQUIRE(!std::contiguous_iterator<iter_t>);
}

TEST_CASE("enumerate satisfies view", "[category]")
{
    using view_t = decltype(enumerate(std::declval<std::vector<int>&>()));
    STATIC_REQUIRE(std::ranges::view<view_t>);
}

// TEST_CASE("enumerate is common_range for common underlying range", "[category]")
// {
//     using view_t = decltype(enumerate(std::declval<std::vector<int>&>()));
//     STATIC_REQUIRE(std::ranges::common_range<view_t>);
// }
//
// TEST_CASE("enumerate is not common_range for non-common underlying range", "[category]")
// {
//     using view_t = decltype(enumerate(
//         std::declval<std::ranges::take_while_view<std::ranges::ref_view<std::vector<int>>, bool (*)(int)>&>()));
//     STATIC_REQUIRE(!std::ranges::common_range<view_t>);
// }
//
// TEST_CASE("enumerate supports const begin/end for const-iterable range", "[category]")
// {
//     std::vector<int> v = {1, 2, 3};
//     const auto view = enumerate(v);
//     using const_view_t = std::remove_reference_t<decltype(view)>;
//     STATIC_REQUIRE(std::ranges::range<const_view_t>);
//     std::vector<std::pair<size_t, int>> result;
//     for (auto [i, val] : view)
//     {
//         result.emplace_back(i, val);
//     }
//     REQUIRE(result.size() == 3);
//     REQUIRE(result[0] == std::pair{0UL, 1});
//     REQUIRE(result[2] == std::pair{2UL, 3});
// }
//
// TEST_CASE("enumerate const view yields const references", "[category]")
// {
//     std::vector<int> v = {1, 2, 3};
//     const auto view = enumerate(v);
//     for (auto [i, val] : view)
//     {
//         // val should be const int& when iterating a const view
//         STATIC_REQUIRE(std::is_const_v<std::remove_reference_t<decltype(val)>>);
//     }
// }
//
// TEST_CASE("enumerate const vs non-const view yield different reference types", "[category]")
// {
//     std::vector<int> v = {1, 2, 3};
//     auto view = enumerate(v);
//     const auto const_view = enumerate(v);
//
//     auto it = view.begin();
//     auto const_it = const_view.begin();
//
//     // Non-const view should yield non-const references
//     auto [i, val] = *it;
//     STATIC_REQUIRE(!std::is_const_v<std::remove_reference_t<decltype(val)>>);
//
//     // Const view should yield const references
//     auto [ci, cval] = *const_it;
//     STATIC_REQUIRE(std::is_const_v<std::remove_reference_t<decltype(cval)>>);
// }

TEST_CASE("enumerate is sized_range for sized underlying range", "[category]")
{
    using view_t = decltype(enumerate(std::declval<std::vector<int>&>()));
    STATIC_REQUIRE(std::ranges::sized_range<view_t>);
}

TEST_CASE("enumerate is not sized_range for non-sized underlying range", "[category]")
{
    using view_t = decltype(enumerate(std::declval<std::forward_list<int>&>()));
    STATIC_REQUIRE(!std::ranges::sized_range<view_t>);
}

TEST_CASE("enumerate size() semantics", "[size]")
{
    std::vector<int> v = {1, 2, 3, 4, 5};
    REQUIRE(enumerate(v).size() == 5);
}

TEST_CASE("enumerate size() is 0 for empty range", "[size]")
{
    std::vector<int> v;
    REQUIRE(enumerate(v).size() == 0);
}

TEST_CASE("enumerate bidirectional: pre-decrement", "[bidirectional]")
{
    std::list<int> l = {10, 20, 30, 40};
    auto view = enumerate(l);
    auto it = view.begin();
    ++it;
    ++it;
    REQUIRE(*it == std::pair{2UL, 30});
    --it;
    REQUIRE(*it == std::pair{1UL, 20});
    --it;
    REQUIRE(*it == std::pair{0UL, 10});
}

TEST_CASE("enumerate bidirectional: post-decrement", "[bidirectional]")
{
    std::list<int> l = {10, 20, 30};
    auto view = enumerate(l);
    auto it = view.begin();
    ++it;
    auto prev = it--;
    REQUIRE(*prev == std::pair{1UL, 20});
    REQUIRE(*it == std::pair{0UL, 10});
}

TEST_CASE("enumerate random access: operator[]", "[random_access]")
{
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto view = enumerate(v);
    auto it = view.begin();
    REQUIRE(it[0] == std::pair(0UL, 1));
    REQUIRE(it[1] == std::pair(1UL, 2));
    REQUIRE(it[2] == std::pair(2UL, 3));
}

TEST_CASE("enumerate random access: operator+", "[random_access]")
{
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto view = enumerate(v);
    auto it = view.begin();
    REQUIRE(*(it + 3) == std::pair(3UL, 4));
    REQUIRE(*(2 + it) == std::pair(2UL, 3));
}

TEST_CASE("enumerate random access: operator-", "[random_access]")
{
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto view = enumerate(v);
    auto it = view.begin() + 3;
    REQUIRE(*(it - 2) == std::pair(1UL, 2));
}

TEST_CASE("enumerate random access: operator+=/-=", "[random_access]")
{
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto view = enumerate(v);
    auto it = view.begin();
    it += 3;
    REQUIRE(*it == std::pair(3UL, 4));
    it -= 2;
    REQUIRE(*it == std::pair(1UL, 2));
}

TEST_CASE("enumerate random access: iterator difference", "[random_access]")
{
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto view = enumerate(v);
    auto a = view.begin();
    auto b = view.begin() + 3;
    REQUIRE(b - a == 3);
    REQUIRE(a - b == -3);
}

TEST_CASE("enumerate random access: comparison operators", "[random_access]")
{
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto view = enumerate(v);
    auto a = view.begin();
    auto b = view.begin() + 2;
    REQUIRE(a < b);
    REQUIRE(b > a);
    REQUIRE(a <= b);
    REQUIRE(b >= a);
    REQUIRE(a <= a);
    REQUIRE(a >= a);
}
// NOLINTEND
