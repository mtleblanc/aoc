# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Role

Claude's intended role in this project is **code review** and **tooling configuration** (CMake, clangd, etc.) — not code generation. Solutions are written by the human; Claude should focus on reviewing correctness, style, and C++ idiom, or helping with build/tooling issues. Do not proactively generate or rewrite solution code.

## Build System

Two pre-configured build directories exist: `build-debug` (default, `-O0`) and `build-release` (`-O3`).

```bash
# Build all targets
cmake --build build-debug

# Build only the main runner
cmake --build build-debug --target aoc

# Build only the verifier
cmake --build build-debug --target verify
```

To reconfigure from scratch (e.g., after adding new source files to an existing year — CMake auto-discovers via `GLOB_RECURSE`):
```bash
cmake -B build-debug -DCMAKE_BUILD_TYPE=Debug
```

## Running Solutions

Input files live at `inputs/<year>/input<DD>` (zero-padded day). Sample inputs at `inputs/<year>/sample<DD>`.

```bash
# Run all solutions for the hardcoded year (currently 2017) with real inputs
./build-debug/aoc

# Include slow solutions (marked IsSlow in solutions.hh)
./build-debug/aoc --slow
```

The active year and day count are set by `constexpr` values in `src/main.cc`. Update `YEAR` and `DAYS` there when starting a new year.

## Tests

```bash
# Run all CTest tests (unit tests + verify)
ctest --test-dir build-debug

# Run a single test executable directly
./build-debug/test_adjacent

# Run only the solution verifier (checks against answers.json)
./build-debug/verify

# Record current outputs into answers.json
./build-debug/verify --record

# Record only a specific year
./build-debug/verify --record --year 2017
```

## Adding a New Year

Use the generator script, then manually add the year's declarations to `include/solutions.hh` and register the year in `src/main.cc` and `src/verify.cc`:

```bash
./generateYear.sh <year> <max_day>
```

This creates `src/<year>/day01.cc` … `src/<year>/dayNN.cc`, each with the standard stub template.

## Architecture

### Solution Registration (template-based dispatch)

Each day is a `.cc` file in `src/<year>/` that provides an explicit template specialization:

```cpp
template <> Solution solve<YEAR, DAY>(std::istream& input) { ... }
```

All specializations are declared in `include/solutions.hh`. The base template (in `include/aoc.hh`) returns a default-constructed solution, so unimplemented days compile and run silently.

**To change a day's return type** from the default `IntSolution`, add a `SolutionType` specialization in `include/solutions.hh`. Common aliases: `IntSolution`, `LongSolution`, `SsizeSolution`, `StringSolution`. Year-wide overrides can also go in `include/aoc.hh`.

**To mark a day as slow**, add `template <> struct IsSlow<YEAR, DAY> : std::true_type {};` in `include/solutions.hh`.

### Answer Verification

`answers.json` stores expected `[part1, part2]` strings keyed by year and day. `verify` compares live output against these. Running with `--record` updates the file.

### Headers (`include/`)

- `aoc.hh` — `GeneralSolution<T>`, `Solution_t<Y,D>`, `readAllLines`, `readAll`, `slurp`
- `solutions.hh` — all `solve<Y,D>` declarations, `SolutionType` overrides, `IsSlow` markers
- `util.hh` — `mod`, `trim`, `toHex`, `toNum`
- `hash.hh` — OpenSSL MD5/EVP wrapper (`Hash::Hasher`)
- `rangepolyfill.hh` — `aoc::adjacent<N>(range)` and `aoc::enumerate(range)` (C++23 polyfills)
- `matrix.hh` — row-major `Matrix<T>` with Gaussian elimination (`rowReduce`)
- `rational.hh` — rational number type

### Dependencies (fetched via CMake FetchContent)

- **Catch2 v3** — unit testing (`tests/test_*.cc`)
- **nlohmann/json** — used by `verify.cc` for `answers.json`
- **scnlib** — scanf-style input parsing
- **CTRE** — compile-time regex (`ctre::match<PAT>(text)`)
- **OpenSSL** — MD5 hashing (must be installed system-wide)

### Typical Solution Pattern

```cpp
#include "aoc.hh"
// optionally: #include "util.hh", "rangepolyfill.hh", <ctre.hpp>, <scn/scan.h>

namespace aoc {
constexpr size_t YEAR = 2017;
constexpr size_t DAY = 8;
using Solution = Solution_t<YEAR, DAY>;

template <> Solution solve<YEAR, DAY>(std::istream& input) {
    auto lines = readAllLines(input);
    // ...
    return {part1, part2};
}
} // namespace aoc
```
