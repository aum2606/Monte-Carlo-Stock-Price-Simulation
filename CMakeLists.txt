cmake_minimum_required(VERSION 3.10)
project(MonteCarloSimulation)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(monte_carlo_simulation monte_carlo_simulation.cpp)

# If you're on Windows and using MSVC, you might need to set the following
if(MSVC)
    target_compile_options(monte_carlo_simulation PRIVATE /W4)
else()
    target_compile_options(monte_carlo_simulation PRIVATE -Wall -Wextra)
endif()