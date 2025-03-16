cmake_minimum_required(VERSION 3.14)
project(KnuthGraph)

set(CMAKE_CXX_STANDARD 20)

include(CTest)
include(GoogleTest)

find_package(GTest REQUIRED)    
find_package(Boost REQUIRED)   

add_subdirectory(src)
add_subdirectory(tests)
