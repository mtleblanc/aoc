cmake_minimum_required(VERSION 3.20)
project(AdventOfCode2025 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add debug symbols
set(CMAKE_BUILD_TYPE Debug)

# Generate compile_commands.json for language server
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find all day*.cc files
file(GLOB DAY_SOURCES "day*.cc")

# Create an executable for each day
foreach(DAY_SOURCE ${DAY_SOURCES})
    # Get the filename without extension (e.g., day01)
    get_filename_component(DAY_NAME ${DAY_SOURCE} NAME_WE)

    # Create executable
    add_executable(${DAY_NAME} ${DAY_SOURCE})

    # Enable warnings
    if(MSVC)
        target_compile_options(${DAY_NAME} PRIVATE /W4)
    else()
        target_compile_options(${DAY_NAME} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()
