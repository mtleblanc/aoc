cmake_minimum_required(VERSION 3.20)
project(AdventOfCode2025 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Fetch Catch2 for testing
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.12.0  # Latest stable version
)
FetchContent_MakeAvailable(Catch2)

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

# Add include directory
include_directories(include)

# Find all day*.cc files across all years
file(GLOB_RECURSE DAY_SOURCES "src/*/day*.cc")

# Main executable that runs solutions
add_executable(aoc src/main.cc ${DAY_SOURCES})

# Enable warnings for main executable
if(MSVC)
    target_compile_options(aoc PRIVATE /W4 /WX)
else()
    target_compile_options(aoc PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

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
