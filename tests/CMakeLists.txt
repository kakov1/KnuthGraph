add_executable(test_graph test.cpp)
add_executable(ansgen ansgen.cpp)

set(TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR})

find_program(BASH_EXECUTABLE NAMES bash REQUIRED)

add_test(
  NAME end2end
  COMMAND ${BASH_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/end2end
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(test_graph PUBLIC TEST_DATA_DIR="${TEST_DATA_DIR}")
target_include_directories(test_graph PUBLIC ${CMAKE_SOURCE_DIR}/inc ${Boost_INCLUDE_DIRS})
target_link_libraries(test_graph PRIVATE GTest::gtest) 

target_compile_definitions(ansgen PUBLIC TEST_DATA_DIR="${TEST_DATA_DIR}")
target_include_directories(ansgen PUBLIC ${CMAKE_SOURCE_DIR}/inc)
target_link_libraries(ansgen PUBLIC Boost::program_options) 

gtest_discover_tests(test_graph)
