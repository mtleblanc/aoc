#pragma once

inline auto mod(auto n, auto p)
{
    return (n % p + p) % p;
}