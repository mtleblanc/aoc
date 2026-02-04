// NOLINTBEGIN
#include "rangepolyfill.hh"
#include <catch2/catch_test_macros.hpp>

#include <forward_list>
#include <iterator>
#include <list>
#include <string>
#include <vector>

using namespace aoc;

TEST_CASE("adjacent<2> over string")
{
    std::string s = "abcde";
    std::vector<std::tuple<char, char>> result;
    for (auto [a, b] : adjacent<2>(s))
    {
        result.emplace_back(a, b);
    }
    REQUIRE(result.size() == 4);
    REQUIRE(result[0] == std::tuple{'a', 'b'});
    REQUIRE(result[1] == std::tuple{'b', 'c'});
    REQUIRE(result[2] == std::tuple{'c', 'd'});
    REQUIRE(result[3] == std::tuple{'d', 'e'});
}

TEST_CASE("adjacent<3> over string")
{
    std::string s = "abcd";
    std::vector<std::tuple<char, char, char>> result;
    for (auto [a, b, c] : adjacent<3>(s))
    {
        result.emplace_back(a, b, c);
    }
    REQUIRE(result.size() == 2);
    REQUIRE(result[0] == std::tuple{'a', 'b', 'c'});
    REQUIRE(result[1] == std::tuple{'b', 'c', 'd'});
}

TEST_CASE("adjacent<4> over string")
{
    std::string s = "abcde";
    std::vector<std::tuple<char, char, char, char>> result;
    for (auto [a, b, c, d] : adjacent<4>(s))
    {
        result.emplace_back(a, b, c, d);
    }
    REQUIRE(result.size() == 2);
    REQUIRE(result[0] == std::tuple{'a', 'b', 'c', 'd'});
    REQUIRE(result[1] == std::tuple{'b', 'c', 'd', 'e'});
}

TEST_CASE("adjacent<1> yields each element")
{
    std::string s = "abc";
    std::vector<std::tuple<char>> result;
    for (auto [a] : adjacent<1>(s))
    {
        result.emplace_back(a);
    }
    REQUIRE(result.size() == 3);
    REQUIRE(result[0] == std::tuple{'a'});
    REQUIRE(result[1] == std::tuple{'b'});
    REQUIRE(result[2] == std::tuple{'c'});
}

TEST_CASE("adjacent with exact N elements")
{
    std::string s = "abc";
    std::vector<std::tuple<char, char, char>> result;
    for (auto [a, b, c] : adjacent<3>(s))
    {
        result.emplace_back(a, b, c);
    }
    REQUIRE(result.size() == 1);
    REQUIRE(result[0] == std::tuple{'a', 'b', 'c'});
}

TEST_CASE("adjacent with fewer than N elements is empty")
{
    std::string s = "ab";
    size_t count = 0;
    for ([[maybe_unused]] auto tup : adjacent<3>(s))
    {
        ++count;
    }
    REQUIRE(count == 0);
}

TEST_CASE("adjacent with empty range is empty")
{
    std::string s;
    size_t count = 0;
    for ([[maybe_unused]] auto tup : adjacent<2>(s))
    {
        ++count;
    }
    REQUIRE(count == 0);
}

TEST_CASE("adjacent over vector of ints")
{
    std::vector<int> v = {10, 20, 30, 40, 50};
    std::vector<std::tuple<int, int, int>> result;
    for (auto [a, b, c] : adjacent<3>(v))
    {
        result.emplace_back(a, b, c);
    }
    REQUIRE(result.size() == 3);
    REQUIRE(result[0] == std::tuple{10, 20, 30});
    REQUIRE(result[1] == std::tuple{20, 30, 40});
    REQUIRE(result[2] == std::tuple{30, 40, 50});
}

TEST_CASE("adjacent over view with sentinel")
{
    auto v = std::vector{10, 20, 30, 40, 50};
    auto w = std::views::take_while(v, [](auto n) { return n < 50; });
    std::vector<std::tuple<int, int, int>> result;
    for (auto [a, b, c] : adjacent<3>(w))
    {
        result.emplace_back(a, b, c);
    }
    REQUIRE(result.size() == 2);
    REQUIRE(result[0] == std::tuple{10, 20, 30});
    REQUIRE(result[1] == std::tuple{20, 30, 40});
}

TEST_CASE("adjacent over rvalue view")
{
    std::vector<int> v = {10, 20, 30, 40, 50};
    std::vector<std::tuple<int, int, int>> result;
    for (auto [a, b, c] : adjacent<3>(std::views::take(v, 4)))
    {
        result.emplace_back(a, b, c);
    }
    REQUIRE(result.size() == 2);
    REQUIRE(result[0] == std::tuple{10, 20, 30});
    REQUIRE(result[1] == std::tuple{20, 30, 40});
}

TEST_CASE("adjacent yields references", "[reference]")
{
    std::vector<int> v = {1, 2, 3};
    for (auto [a, b, c] : adjacent<3>(v))
    {
        // a, b, c should be references to the original vector elements
        REQUIRE(&a == &v[0]);
        REQUIRE(&b == &v[1]);
        REQUIRE(&c == &v[2]);
    }
}

TEST_CASE("adjacent allows mutation through reference", "[reference]")
{
    std::vector<int> v = {1, 2, 3};
    for (auto [a, b, c] : adjacent<3>(v))
    {
        a *= 10;
        b *= 10;
        c *= 10;
    }
    REQUIRE(v == std::vector{10, 20, 30});
}

TEST_CASE("adjacent over forward_list uses sentinel path", "[sentinel]")
{
    std::forward_list<int> fl = {1, 2, 3, 4, 5};
    std::vector<std::tuple<int, int, int>> result;
    for (auto [a, b, c] : adjacent<3>(fl))
    {
        result.emplace_back(a, b, c);
    }
    REQUIRE(result.size() == 3);
    REQUIRE(result[0] == std::tuple{1, 2, 3});
    REQUIRE(result[1] == std::tuple{2, 3, 4});
    REQUIRE(result[2] == std::tuple{3, 4, 5});
}

TEST_CASE("adjacent over short forward_list is empty", "[sentinel]")
{
    std::forward_list<int> fl = {1};
    size_t count = 0;
    for ([[maybe_unused]] auto tup : adjacent<3>(fl))
    {
        ++count;
    }
    REQUIRE(count == 0);
}

TEST_CASE("adjacent over empty forward_list is empty", "[sentinel]")
{
    std::forward_list<int> fl;
    size_t count = 0;
    for ([[maybe_unused]] auto tup : adjacent<2>(fl))
    {
        ++count;
    }
    REQUIRE(count == 0);
}

TEST_CASE("adjacent forward_list with exact N elements", "[sentinel]")
{
    std::forward_list<int> fl = {10, 20};
    std::vector<std::tuple<int, int>> result;
    for (auto [a, b] : adjacent<2>(fl))
    {
        result.emplace_back(a, b);
    }
    REQUIRE(result.size() == 1);
    REQUIRE(result[0] == std::tuple{10, 20});
}

TEST_CASE("post-increment iterator")
{
    std::string s = "abcd";
    auto view = adjacent<2>(s);
    auto it = view.begin();
    auto prev = it++;
    REQUIRE(*prev == std::tuple{'a', 'b'});
    REQUIRE(*it == std::tuple{'b', 'c'});
}

TEST_CASE("adjacent satisfies forward_iterator for forward range", "[category]")
{
    using view_t = decltype(adjacent<2>(std::declval<std::forward_list<int>&>()));
    using iter_t = decltype(std::declval<view_t>().begin());
    STATIC_REQUIRE(std::forward_iterator<iter_t>);
    STATIC_REQUIRE(!std::bidirectional_iterator<iter_t>);
    STATIC_REQUIRE(std::ranges::forward_range<view_t>);
    STATIC_REQUIRE(!std::ranges::bidirectional_range<view_t>);
}

TEST_CASE("adjacent satisfies bidirectional_iterator for bidirectional range", "[category]")
{
    using view_t = decltype(adjacent<2>(std::declval<std::list<int>&>()));
    using iter_t = decltype(std::declval<view_t>().begin());
    STATIC_REQUIRE(std::bidirectional_iterator<iter_t>);
    STATIC_REQUIRE(!std::random_access_iterator<iter_t>);
    STATIC_REQUIRE(std::ranges::bidirectional_range<view_t>);
    STATIC_REQUIRE(!std::ranges::random_access_range<view_t>);
}

TEST_CASE("adjacent satisfies random_access_iterator for random access range", "[category]")
{
    using view_t = decltype(adjacent<2>(std::declval<std::vector<int>&>()));
    using iter_t = decltype(std::declval<view_t>().begin());
    STATIC_REQUIRE(std::random_access_iterator<iter_t>);
    STATIC_REQUIRE(std::ranges::random_access_range<view_t>);
}

TEST_CASE("adjacent not contiguous even for contiguous range", "[category]")
{
    using view_t = decltype(adjacent<2>(std::declval<std::vector<int>&>()));
    using iter_t = decltype(std::declval<view_t>().begin());
    STATIC_REQUIRE(std::random_access_iterator<iter_t>);
    STATIC_REQUIRE(!std::contiguous_iterator<iter_t>);
}

TEST_CASE("adjacent satisfies view", "[category]")
{
    using view_t = decltype(adjacent<2>(std::declval<std::vector<int>&>()));
    STATIC_REQUIRE(std::ranges::view<view_t>);
}

TEST_CASE("adjacent is sized_range for sized underlying range", "[category]")
{
    using view_t = decltype(adjacent<2>(std::declval<std::vector<int>&>()));
    STATIC_REQUIRE(std::ranges::sized_range<view_t>);
}

TEST_CASE("adjacent is not sized_range for non-sized underlying range", "[category]")
{
    using view_t = decltype(adjacent<2>(std::declval<std::forward_list<int>&>()));
    STATIC_REQUIRE(!std::ranges::sized_range<view_t>);
}

TEST_CASE("adjacent size() semantics", "[size]")
{
    std::vector<int> v = {1, 2, 3, 4, 5};
    REQUIRE(adjacent<1>(v).size() == 5);
    REQUIRE(adjacent<2>(v).size() == 4);
    REQUIRE(adjacent<3>(v).size() == 3);
    REQUIRE(adjacent<4>(v).size() == 2);
    REQUIRE(adjacent<5>(v).size() == 1);
}

TEST_CASE("adjacent size() is 0 for too-short range", "[size]")
{
    std::vector<int> v = {1, 2};
    REQUIRE(adjacent<3>(v).size() == 0);
    REQUIRE(adjacent<4>(v).size() == 0);
}

TEST_CASE("adjacent size() is 0 for empty range", "[size]")
{
    std::vector<int> v;
    REQUIRE(adjacent<2>(v).size() == 0);
}

TEST_CASE("adjacent bidirectional: pre-decrement", "[bidirectional]")
{
    std::list<int> l = {10, 20, 30, 40};
    auto view = adjacent<2>(l);
    auto it = view.begin();
    ++it;
    ++it;
    REQUIRE(*it == std::tuple{30, 40});
    --it;
    REQUIRE(*it == std::tuple{20, 30});
    --it;
    REQUIRE(*it == std::tuple{10, 20});
}

TEST_CASE("adjacent bidirectional: post-decrement", "[bidirectional]")
{
    std::list<int> l = {10, 20, 30};
    auto view = adjacent<2>(l);
    auto it = view.begin();
    ++it;
    auto prev = it--;
    REQUIRE(*prev == std::tuple{20, 30});
    REQUIRE(*it == std::tuple{10, 20});
}

TEST_CASE("adjacent random access: operator[]", "[random_access]")
{
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto view = adjacent<3>(v);
    auto it = view.begin();
    REQUIRE(it[0] == std::tuple{1, 2, 3});
    REQUIRE(it[1] == std::tuple{2, 3, 4});
    REQUIRE(it[2] == std::tuple{3, 4, 5});
}

TEST_CASE("adjacent random access: operator+", "[random_access]")
{
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto view = adjacent<2>(v);
    auto it = view.begin();
    REQUIRE(*(it + 3) == std::tuple{4, 5});
    REQUIRE(*(2 + it) == std::tuple{3, 4});
}

TEST_CASE("adjacent random access: operator-", "[random_access]")
{
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto view = adjacent<2>(v);
    auto it = view.begin() + 3;
    REQUIRE(*(it - 2) == std::tuple{2, 3});
}

TEST_CASE("adjacent random access: operator+=/-=", "[random_access]")
{
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto view = adjacent<2>(v);
    auto it = view.begin();
    it += 3;
    REQUIRE(*it == std::tuple{4, 5});
    it -= 2;
    REQUIRE(*it == std::tuple{2, 3});
}

TEST_CASE("adjacent random access: iterator difference", "[random_access]")
{
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto view = adjacent<2>(v);
    auto a = view.begin();
    auto b = view.begin() + 3;
    REQUIRE(b - a == 3);
    REQUIRE(a - b == -3);
}

TEST_CASE("adjacent random access: comparison operators", "[random_access]")
{
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto view = adjacent<2>(v);
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
