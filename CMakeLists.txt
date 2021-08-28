cmake_minimum_required(VERSION 3.16)

project(datafeed)

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(BOOST_MIN_VERSION "1.70.0")

find_package(Boost REQUIRED COMPONENTS iostreams)

add_executable(datafeed src/main.cpp src/parser.cpp)
include_directories(include)
target_include_directories(datafeed PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(datafeed ${Boost_LIBRARIES})
