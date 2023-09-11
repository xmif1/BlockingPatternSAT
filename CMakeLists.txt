cmake_minimum_required(VERSION 3.16)
project(BlockingPatternSAT)

set(CMAKE_CXX_STANDARD 14)

find_package(ALLLSatisfiabilitySolver CONFIG REQUIRED)

add_executable(BlockingPatternSAT main.cpp
        BlockingPattern/blocking_pattern.h)
target_link_libraries(BlockingPatternSAT curses ALLLSatisfiabilitySolver)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -DNDEBUG") # to specify compiler optimisations add -Ofast -DNDEBUG