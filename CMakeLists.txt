cmake_minimum_required(VERSION 3.14)
project(KnuthGraph)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

find_package(GTest REQUIRED)    

add_subdirectory(src)
#add_subdirectory(tests)
#add_subdirectory(benchmark)
