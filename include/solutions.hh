#pragma once
#include "aoc.hh"

namespace aoc
{

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers)
template <> struct SolutionType<2016, 2>
{
    using type = StringSolution;
};

template <> struct SolutionType<2016, 5>
{
    using type = StringSolution;
};

template <> struct SolutionType<2016, 6>
{
    using type = StringSolution;
};

template <> struct SolutionType<2016, 8>
{
    using type = StringSolution;
};

template <> struct SolutionType<2016, 9>
{
    using type = LongSolution;
};

template <> struct SolutionType<2016, 16>
{
    using type = StringSolution;
};

template <> struct SolutionType<2016, 17>
{
    using type = StringSolution;
};

template <> struct SolutionType<2016, 20>
{
    using type = LongSolution;
};

template <> struct SolutionType<2016, 21>
{
    using type = StringSolution;
};

template <size_t Y, size_t D> struct IsSlow : std::false_type
{
};

template <> struct IsSlow<2016, 5> : std::true_type
{
};
template <> struct IsSlow<2016, 14> : std::true_type
{
};

template <> Solution_t<2015, 1> solve<2015, 1>(std::istream& input);
template <> Solution_t<2015, 2> solve<2015, 2>(std::istream& input);
template <> Solution_t<2015, 3> solve<2015, 3>(std::istream& input);
template <> Solution_t<2015, 4> solve<2015, 4>(std::istream& input);
template <> Solution_t<2015, 5> solve<2015, 5>(std::istream& input);
template <> Solution_t<2015, 6> solve<2015, 6>(std::istream& input);
template <> Solution_t<2015, 7> solve<2015, 7>(std::istream& input);
template <> Solution_t<2015, 8> solve<2015, 8>(std::istream& input);
template <> Solution_t<2015, 9> solve<2015, 9>(std::istream& input);
template <> Solution_t<2015, 10> solve<2015, 10>(std::istream& input);
template <> Solution_t<2015, 11> solve<2015, 11>(std::istream& input);
template <> Solution_t<2015, 12> solve<2015, 12>(std::istream& input);
template <> Solution_t<2015, 13> solve<2015, 13>(std::istream& input);
template <> Solution_t<2015, 14> solve<2015, 14>(std::istream& input);
template <> Solution_t<2015, 15> solve<2015, 15>(std::istream& input);
template <> Solution_t<2015, 16> solve<2015, 16>(std::istream& input);
template <> Solution_t<2015, 17> solve<2015, 17>(std::istream& input);
template <> Solution_t<2015, 18> solve<2015, 18>(std::istream& input);
template <> Solution_t<2015, 19> solve<2015, 19>(std::istream& input);
template <> Solution_t<2015, 20> solve<2015, 20>(std::istream& input);
template <> Solution_t<2015, 21> solve<2015, 21>(std::istream& input);
template <> Solution_t<2015, 22> solve<2015, 22>(std::istream& input);
template <> Solution_t<2015, 23> solve<2015, 23>(std::istream& input);
template <> Solution_t<2015, 24> solve<2015, 24>(std::istream& input);
template <> Solution_t<2015, 25> solve<2015, 25>(std::istream& input);

template <> Solution_t<2016, 1> solve<2016, 1>(std::istream& input);
template <> Solution_t<2016, 2> solve<2016, 2>(std::istream& input);
template <> Solution_t<2016, 3> solve<2016, 3>(std::istream& input);
template <> Solution_t<2016, 4> solve<2016, 4>(std::istream& input);
template <> Solution_t<2016, 5> solve<2016, 5>(std::istream& input);
template <> Solution_t<2016, 6> solve<2016, 6>(std::istream& input);
template <> Solution_t<2016, 7> solve<2016, 7>(std::istream& input);
template <> Solution_t<2016, 8> solve<2016, 8>(std::istream& input);
template <> Solution_t<2016, 9> solve<2016, 9>(std::istream& input);
template <> Solution_t<2016, 10> solve<2016, 10>(std::istream& input);
template <> Solution_t<2016, 11> solve<2016, 11>(std::istream& input);
template <> Solution_t<2016, 12> solve<2016, 12>(std::istream& input);
template <> Solution_t<2016, 13> solve<2016, 13>(std::istream& input);
template <> Solution_t<2016, 14> solve<2016, 14>(std::istream& input);
template <> Solution_t<2016, 15> solve<2016, 15>(std::istream& input);
template <> Solution_t<2016, 16> solve<2016, 16>(std::istream& input);
template <> Solution_t<2016, 17> solve<2016, 17>(std::istream& input);
template <> Solution_t<2016, 18> solve<2016, 18>(std::istream& input);
template <> Solution_t<2016, 19> solve<2016, 19>(std::istream& input);
template <> Solution_t<2016, 20> solve<2016, 20>(std::istream& input);
template <> Solution_t<2016, 21> solve<2016, 21>(std::istream& input);
template <> Solution_t<2016, 22> solve<2016, 22>(std::istream& input);
template <> Solution_t<2016, 23> solve<2016, 23>(std::istream& input);
template <> Solution_t<2016, 24> solve<2016, 24>(std::istream& input);
template <> Solution_t<2016, 25> solve<2016, 25>(std::istream& input);

template <> Solution_t<2024, 1> solve<2024, 1>(std::istream& input);
template <> Solution_t<2024, 2> solve<2024, 2>(std::istream& input);
template <> Solution_t<2024, 3> solve<2024, 3>(std::istream& input);
template <> Solution_t<2024, 4> solve<2024, 4>(std::istream& input);
template <> Solution_t<2024, 5> solve<2024, 5>(std::istream& input);
template <> Solution_t<2024, 6> solve<2024, 6>(std::istream& input);
template <> Solution_t<2024, 7> solve<2024, 7>(std::istream& input);
template <> Solution_t<2024, 8> solve<2024, 8>(std::istream& input);
template <> Solution_t<2024, 9> solve<2024, 9>(std::istream& input);
template <> Solution_t<2024, 10> solve<2024, 10>(std::istream& input);
template <> Solution_t<2024, 11> solve<2024, 11>(std::istream& input);
template <> Solution_t<2024, 12> solve<2024, 12>(std::istream& input);
template <> Solution_t<2024, 13> solve<2024, 13>(std::istream& input);
template <> Solution_t<2024, 14> solve<2024, 14>(std::istream& input);
template <> Solution_t<2024, 15> solve<2024, 15>(std::istream& input);
template <> Solution_t<2024, 16> solve<2024, 16>(std::istream& input);
template <> Solution_t<2024, 17> solve<2024, 17>(std::istream& input);
template <> Solution_t<2024, 18> solve<2024, 18>(std::istream& input);
template <> Solution_t<2024, 19> solve<2024, 19>(std::istream& input);
template <> Solution_t<2024, 20> solve<2024, 20>(std::istream& input);
template <> Solution_t<2024, 21> solve<2024, 21>(std::istream& input);
template <> Solution_t<2024, 22> solve<2024, 22>(std::istream& input);
template <> Solution_t<2024, 23> solve<2024, 23>(std::istream& input);
template <> Solution_t<2024, 24> solve<2024, 24>(std::istream& input);
template <> Solution_t<2024, 25> solve<2024, 25>(std::istream& input);

template <> Solution_t<2025, 1> solve<2025, 1>(std::istream& input);
template <> Solution_t<2025, 2> solve<2025, 2>(std::istream& input);
template <> Solution_t<2025, 3> solve<2025, 3>(std::istream& input);
template <> Solution_t<2025, 4> solve<2025, 4>(std::istream& input);
template <> Solution_t<2025, 5> solve<2025, 5>(std::istream& input);
template <> Solution_t<2025, 6> solve<2025, 6>(std::istream& input);
template <> Solution_t<2025, 7> solve<2025, 7>(std::istream& input);
template <> Solution_t<2025, 8> solve<2025, 8>(std::istream& input);
template <> Solution_t<2025, 9> solve<2025, 9>(std::istream& input);
template <> Solution_t<2025, 10> solve<2025, 10>(std::istream& input);
template <> Solution_t<2025, 11> solve<2025, 11>(std::istream& input);
template <> Solution_t<2025, 12> solve<2025, 12>(std::istream& input);
// NOLINTEND

} // namespace aoc
