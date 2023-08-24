cmake_minimum_required(VERSION 3.16)
project(BlockingPatternSAT)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED COMPONENTS JSON)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

find_package(OpenMP)
if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

find_package(ALLLSatisfiabilitySolver CONFIG REQUIRED)

add_executable(BlockingPatternSAT main.cpp
        ClauseGenerator/generator.h
        BlockingPattern/blocking_pattern.h)
target_link_libraries(BlockingPatternSAT Boost::json OpenMP::OpenMP_CXX ALLLSatisfiabilitySolver)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -DNDEBUG") # to specify compiler optimisations add -Ofast -DNDEBUG