#pragma once

#include <charconv>
#include <iomanip>
#include <ios>
#include <sstream>
#include <string>
#include <string_view>

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

template <typename T> T toNum(std::string_view sv)
{
    T val{};
    std::from_chars(sv.begin(), sv.end(), val);
    return val;
}
} // namespace aoc
