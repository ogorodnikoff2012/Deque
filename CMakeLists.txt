cmake_minimum_required(VERSION 3.0.2)
project(Deque)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -O2") #-g -Wall -Wextra -Wpedantic")

include_directories(include)
link_directories(lib)
set(SOURCE_FILES main.cpp)
add_executable(Deque ${SOURCE_FILES})
target_link_libraries(Deque gtest)

# add_executable(DequeTest tests/44573.cpp)