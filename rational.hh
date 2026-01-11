#pragma once
#include <ostream>
#include <stdexcept>
#include <utility>
template <typename T> struct Rational
{
    // Invariant d>0, maintained by reduce();
    // No overflow protection
    Rational(T n = T{}, T d = T{1}) : n{n}, d{d}
    {
        if (d == 0)
        {
            throw std::invalid_argument("Denominator cannot be zero");
        }
        reduce();
    }

    explicit operator T() {
        return n/d;
    }

    // everything is passed by value since the representation is small
    bool operator==(Rational<T> other) const
    {
        return other.d * n == other.n * d;
    }
    bool operator!=(Rational<T> other) const
    {
        return !(*this == other);
    }
    bool operator<(Rational<T> other) const
    {
        // if d were allowed to be negative we'd need to check here and possibly reverse the order
        return n * other.d < d * other.n;
    }
    bool operator>(Rational<T> other) const
    {
        return other < *this;
    }
    bool operator<=(Rational<T> other) const
    {
        return !(*this > other);
    }
    bool operator>=(Rational<T> other) const
    {
        return !(*this < other);
    }
    Rational<T>& operator+=(Rational<T> other)
    {
        n *= other.d;
        n += other.n * d;
        d *= other.d;
        reduce();
        return *this;
    }
    Rational<T>& operator-=(Rational<T> other)
    {
        n *= other.d;
        n -= other.n * d;
        d *= other.d;
        reduce();
        return *this;
    }
    Rational<T>& operator*=(Rational<T> other)
    {
        n *= other.n;
        d *= other.d;
        reduce();
        return *this;
    }
    Rational<T>& operator/=(Rational<T> other)
    {
        if (other.n == 0)
        {
            throw std::invalid_argument("Divide by zero exception");
        }
        n *= other.d;
        d *= other.n;
        reduce();
        return *this;
    }
    friend std::ostream& operator<<(std::ostream& os, Rational<T> r)
    {
        if (r.d == 1)
        {
            return os << r.n;
        }
        return os << r.n << '/' << r.d;
    }

  private:
    static T gcdPos(T a, T b)
    {
        if (b == 0)
        {
            return a;
        }
        return gcdPos(b, a % b);
    }
    static T gcd(T a, T b)
    {
        if (a < 0)
        {
            a = -a;
        }
        if (b < 0)
        {
            b = -b;
        }
        if (a < b)
        {
            std::swap(a, b);
        }
        return gcdPos(a, b);
    }
    void reduce()
    {
        T g = gcd(n, d);
        n /= g;
        d /= g;
        if (d < 0)
        {
            n = -n;
            d = -d;
        }
    }
    T n;
    T d;
};

template <typename T> Rational<T> operator+(Rational<T> l, Rational<T> r)
{
    return l += r;
}
template <typename T> Rational<T> operator+(Rational<T> l, T r)
{
    return l += Rational<T>{r};
}
template <typename T> Rational<T> operator+(T l, Rational<T> r)
{
    return Rational<T>{l} += r;
}
template <typename T> Rational<T> operator-(Rational<T> l, Rational<T> r)
{
    return l -= r;
}
template <typename T> Rational<T> operator-(Rational<T> l, T r)
{
    return l -= Rational<T>{r};
}
template <typename T> Rational<T> operator-(T l, Rational<T> r)
{
    return Rational<T>{l} -= r;
}
template <typename T> Rational<T> operator-(Rational<T> l)
{
    return Rational<T>{} - l;
}
template <typename T> Rational<T> operator*(Rational<T> l, Rational<T> r)
{
    return l *= r;
}
template <typename T> Rational<T> operator*(Rational<T> l, T r)
{
    return l *= Rational<T>{r};
}
template <typename T> Rational<T> operator*(T l, Rational<T> r)
{
    return Rational<T>{l} *= r;
}
template <typename T> Rational<T> operator/(Rational<T> l, Rational<T> r)
{
    return l /= r;
}
template <typename T> Rational<T> operator/(Rational<T> l, T r)
{
    return l /= Rational<T>{r};
}
template <typename T> Rational<T> operator/(T l, Rational<T> r)
{
    return Rational<T>{l} /= r;
}