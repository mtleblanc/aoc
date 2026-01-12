#include <catch2/catch_test_macros.hpp>
#include <stdexcept>
// NOLINTBEGIN
#include "rational.hh"

using R = Rational<int>;

TEST_CASE("Basic arithmetic")
{
    R r1{1, 2};
    R r2{1, 3};
    REQUIRE(r1 + r2 == R{5, 6});
    REQUIRE(r1 - r2 == R{1, 6});
}

TEST_CASE("Multiplication and division")
{
    R r1{2, 3};
    R r2{3, 4};
    REQUIRE(r1 * r2 == R{1, 2});
    REQUIRE(r1 / r2 == R{8, 9});
}

TEST_CASE("Division by zero throws")
{
    R r1{1, 2};
    R r2{0, 1};
    REQUIRE_THROWS_AS(r1 / r2, std::invalid_argument);
}

TEST_CASE("Reduction to lowest terms")
{
    R r1{2, 4};
    R r2{1, 2};
    REQUIRE(r1 == r2);
    REQUIRE(R{6, 9} == R{2, 3});
    REQUIRE(R{10, 15} == R{2, 3});
}

TEST_CASE("Negative numbers")
{
    R r1{-1, 2};
    R r2{1, -2};
    REQUIRE(r1 == r2);
    REQUIRE(R{-1, -2} == R{1, 2});
    REQUIRE(R{-2, 3} + R{2, 3} == R{0, 1});
}

TEST_CASE("Zero handling")
{
    R r1{0, 1};
    R r2{0, 5};
    REQUIRE(r1 == r2);
    REQUIRE(r1 + R{1, 2} == R{1, 2});
    REQUIRE(r1 * R{5, 7} == R{0, 1});
}

TEST_CASE("Comparison operators")
{
    R r1{1, 2};
    R r2{1, 3};
    R r3{2, 4};

    REQUIRE(r2 < r1);
    REQUIRE(r1 > r2);
    REQUIRE(r1 >= r3);
    REQUIRE(r1 <= r3);
    REQUIRE(r1 != r2);
    REQUIRE(R{3, 4} > R{2, 3});
}

TEST_CASE("Comparison operators on negatives")
{
    R r1{1, 2};
    R r2{1, 3};
    R r3{2, 4};

    REQUIRE(-r2 > -r1);
    REQUIRE(-r1 < -r2);
    REQUIRE(-r1 <= -r3);
    REQUIRE(-r1 >= -r3);
    REQUIRE(-r1 != -r2);
    REQUIRE(R{3, -4} < R{-2, 3});
}

TEST_CASE("Compound assignment operators")
{
    R r{1, 2};
    r += R{1, 3};
    REQUIRE(r == R{5, 6});

    r -= R{1, 6};
    REQUIRE(r == R{2, 3});

    r *= R{3, 2};
    REQUIRE(r == R{1, 1});

    r /= R{2, 3};
    REQUIRE(r == R{3, 2});
}

TEST_CASE("Whole numbers")
{
    R r1{5};
    R r2{10, 2};
    REQUIRE(r1 == r2);
    REQUIRE(r1 + R{3, 1} == R{8, 1});
    REQUIRE(R{7, 1} * R{2, 1} == R{14, 1});
}

TEST_CASE("Chained operations")
{
    R result = R{1, 2} + R{1, 3} + R{1, 6};
    REQUIRE(result == R{1, 1});

    result = R{2, 1} - R{1, 2} - R{1, 4};
    REQUIRE(result == R{5, 4});

    result = R{2, 3} * R{3, 4} * R{4, 5};
    REQUIRE(result == R{2, 5});
}

TEST_CASE("Mixed rational and integer operations")
{
    R r{1, 2};
    REQUIRE(r + 3 == R{7, 2});
    REQUIRE(3 + r == R{7, 2});
    REQUIRE(r - 2 == R{-3, 2});
    REQUIRE(5 - r == R{9, 2});
    REQUIRE(r * 4 == R{2, 1});
    REQUIRE(4 * r == R{2, 1});
    REQUIRE(r / 2 == R{1, 4});
    REQUIRE(6 / r == R{12, 1});
}

// NOLINTEND