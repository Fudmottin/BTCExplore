cmake_minimum_required(VERSION 3.15)
project(BTCExplore VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Boost
find_package(Boost 1.85 REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})

# Add cppcodec as a submodule
add_subdirectory(external/cppcodec)

# Find nlohmann_json
find_package(nlohmann_json 3.0 REQUIRED)

# Add executable and source files
add_executable(BTCExplore
    src/main.cpp
    src/configfile.cpp
    src/btcclient.cpp
    src/btcserver.cpp
)

# Link libraries
target_link_libraries(BTCExplore
    PRIVATE
    Boost::system
    cppcodec
    nlohmann_json::nlohmann_json
)

# Include directories
target_include_directories(BTCExplore
    PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/external/cppcodec
)

# Add options for warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(BTCExplore PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    target_compile_options(BTCExplore PRIVATE /W4 /WX)
endif()

