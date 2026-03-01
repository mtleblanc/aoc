#pragma once
#include <cstddef>
#include <istream>
#include <string>
#include <utility>

namespace aoc
{

/// Runs the registered solver for (year, day) and returns {part1, part2} as strings.
/// Returns {"", ""} for unregistered (year, day) pairs.
std::pair<std::string, std::string> runSolver(size_t year, size_t day, std::istream& input);

} // namespace aoc
