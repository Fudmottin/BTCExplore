cmake_minimum_required(VERSION 3.0)
project(BTCExplore)

set(CMAKE_CXX_STANDARD 20)

# Find Boost
find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})

# Add cppcodec as a submodule
add_subdirectory(external/cppcodec)

# Find nlohmann_json
find_package(nlohmann_json 3.0 REQUIRED)

add_executable(BTCExplore src/main.cpp src/configfile.cpp)
target_link_libraries(BTCExplore ${Boost_LIBRARIES})

# Link against cppcodec
target_link_libraries(BTCExplore cppcodec)

# Link against nlohmann_json
target_link_libraries(BTCExplore nlohmann_json::nlohmann_json)

