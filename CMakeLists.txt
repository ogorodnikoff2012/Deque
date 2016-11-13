cmake_minimum_required(VERSION 3.0.2)
project(Deque)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -O2 -march=native")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread  -g -Wall -Wextra -Wpedantic")

add_subdirectory(${CMAKE_SOURCE_DIR}/googletest/googletest)

include_directories(include googletest/googletest/include)
link_directories(${LIBRARY_OUTUT_PATH})
set(SOURCE_FILES main.cpp tests/PushPopTest.cpp tests/DummyTest.cpp tests/IteratorTest.cpp tests/AdaptorTest.cpp)
add_executable(Deque ${SOURCE_FILES})
target_link_libraries(Deque gtest)
