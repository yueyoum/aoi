cmake_minimum_required(VERSION 3.5)
project(TestOne)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -O2")

set(SOURCE_FILES main.cpp)
add_executable(TestOne ${SOURCE_FILES})
