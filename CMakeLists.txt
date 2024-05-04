cmake_minimum_required(VERSION 3.0)
project(BTCExplore)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(BTCExplore src/main.cpp)
target_link_libraries(BTCExplore ${Boost_LIBRARIES})

