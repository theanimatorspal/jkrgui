cmake_minimum_required(VERSION 3.28)
include("../../CMakeCommons.cmake")
file(GLOB_RECURSE ksaicore_SRC CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library(ksaicore STATIC ${ksaicore_SRC})

PrecompileStdHeaders(ksaicore)

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(ksaicore PRIVATE SDL3::SDL3)

