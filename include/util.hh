#pragma once

#include <bitset>
#include <charconv>
#include <ctre.hpp>
#include <iomanip>
#include <ios>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace aoc
{
constexpr auto mod(auto n, auto p)
{
    return (n % p + p) % p;
}

inline std::string_view trim(std::string_view s)
{
    auto start = s.find_first_not_of(" \t\n\r");
    if (start == std::string_view::npos)
    {
        return {};
    }
    auto end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

inline std::string toHex(const auto& n, int padding = 0)
{
    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(padding) << std::hex << n;
    return ss.str();
}

template <size_t BITS> inline std::string toBin(const auto& n)
{
    return std::bitset<BITS>(n).to_string();
}

template <typename T = int> T toNum(std::string_view sv)
{
    T val{};
    std::from_chars(sv.begin(), sv.end(), val);
    return val;
}

template <typename T = int> inline std::vector<T> readNumbers(std::string_view s)
{
    static auto constexpr NUMS = ctll::fixed_string(R"((-?\d+))");
    return ctre::search_all<NUMS>(s) |
           std::views::transform([](auto m)
                                 { return m.template get<1>().template to_number<T>(); }) |
           std::ranges::to<std::vector>();
}
} // namespace aoc
