#include <catch2/catch_test_macros.hpp>
//NOLINTBEGIN
#include "../src/2025/day10.cc"

TEST_CASE("Reduce Helper", "[linalg]") {
    std::vector<std::vector<float>> cols;
    std::vector<float> coeffs {1.0f};
    std::vector<float> orig {2.f, 3.f, 4.f };
    cols.push_back({4.f, 3.f, 2.f});
    aoc::reduceHelper(cols, coeffs, orig);
    REQUIRE( orig == std::vector{-2.f, 0.f, 2.f});

    cols.push_back({7.f, 1.f, 4.f});
    coeffs.push_back(-1.f);
    aoc::reduceHelper(cols, coeffs, orig);
    REQUIRE( orig == std::vector{1.f,-2.f, 4.f });
}

TEST_CASE("Matrix", "[linalg]") {
    Matrix<float> m1{ {1,2,3}, {4,5,6}};
    Matrix<float> m2 {2,3};
    m2(0,0) = 1;
    m2(0,1) = 2;
    m2(0,2) = 3;
    m2(1,0) = 4;
    m2(1, 1) = 5;
    m2(1, 2) = 6;
    REQUIRE( m1 == m2 );

    m1.rowReduce();
    REQUIRE(m1 == Matrix<float>{{1,0,-1}, {0,1, 2}});
}

//NOLINTEND