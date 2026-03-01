cmake_minimum_required(VERSION 3.25)
project(AdventOfCode2025 CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Fetch dependencies
include(FetchContent)

# Fetch Catch2 for testing
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.12.0
)

# Fetch nlohmann/json for JSON parsing
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.12.0
)

# Fetch scnlib for input parsing
FetchContent_Declare(
  scn
  GIT_REPOSITORY https://github.com/eliaskosunen/scnlib.git
  GIT_TAG        v4.0.1
  SYSTEM
)

# Fetch CTRE for compile-time regular expressions
FetchContent_Declare(
  ctre
  GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
  GIT_TAG        v3.9.0
  SYSTEM
)

FetchContent_MakeAvailable(Catch2 json scn ctre)

# Add Catch2's CMake modules to the path
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Configure flags for each build type
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fno-inline")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")

# Generate compile_commands.json for language server
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Optional: Run clang-tidy during build (uncomment to enable)
# find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
# if(CLANG_TIDY_EXE)
#     set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
# endif()

# Find OpenSSL (install via: brew install openssl)
find_package(OpenSSL QUIET)
if(NOT OpenSSL_FOUND)
    message(FATAL_ERROR
        "OpenSSL not found. Please install it:\n"
        "  macOS:  brew install openssl\n"
        "  Ubuntu: sudo apt install libssl-dev\n"
        "  Fedora: sudo dnf install openssl-devel"
    )
endif()

# Add include directory
include_directories(include)

# Find all day*.cc files across all years
file(GLOB_RECURSE DAY_SOURCES "src/*/day*.cc")

# Static library containing all solutions (avoids compiling day*.cc twice)
add_library(solutions STATIC ${DAY_SOURCES})
target_link_libraries(solutions PUBLIC OpenSSL::Crypto nlohmann_json::nlohmann_json scn::scn ctre::ctre)
if(MSVC)
    target_compile_options(solutions PRIVATE /W4 /WX)
else()
    target_compile_options(solutions PRIVATE -Wall -Wextra -Wpedantic
        $<$<NOT:$<CONFIG:Debug>>:-Werror>)
endif()

# Main executable that runs solutions
add_executable(aoc src/main.cc)
target_link_libraries(aoc PRIVATE solutions)

# Verify executable for checking solutions against expected answers
add_executable(verify src/verify.cc src/dispatch.cc)
target_link_libraries(verify PRIVATE solutions nlohmann_json::nlohmann_json)

# Enable testing
enable_testing()

# Find all test files in tests directory
file(GLOB TEST_SOURCES "tests/test_*.cc")

# Create test executable for each test file
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE Catch2::Catch2WithMain)
    target_compile_definitions(${TEST_NAME} PRIVATE TESTING)

    # Enable warnings for tests too
    if(MSVC)
        target_compile_options(${TEST_NAME} PRIVATE /W4)
    else()
        target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    # Register with CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

# Register verify as a CTest test (runs from project root for answers.json access)
add_test(NAME verify_solutions COMMAND verify)
set_tests_properties(verify_solutions PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
