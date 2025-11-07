
cmake_minimum_required(VERSION 3.28)
project(jkrgui)
include("CMakeCommons.cmake")
RunOnce()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("src/ksaicore/include")
add_subdirectory("src/ksaicore")

include_directories("src/ksaieng/include")
add_subdirectory("src/ksaieng")

include_directories("src/jkrgui/include")
add_subdirectory("src/jkrgui")
