cmake_minimum_required(VERSION 3.16)
project(BlockingPatternSAT)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED COMPONENTS program_options)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

find_package(ALLLSatisfiabilitySolver CONFIG REQUIRED)

add_executable(BlockingPatternSAT main.cpp
        BlockingPattern/blocking_pattern.h)
target_link_libraries(BlockingPatternSAT Boost::program_options ALLLSatisfiabilitySolver)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -DNDEBUG")
