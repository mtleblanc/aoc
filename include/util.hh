#pragma once

constexpr auto mod(auto n, auto p)
{
    return (n % p + p) % p;
}