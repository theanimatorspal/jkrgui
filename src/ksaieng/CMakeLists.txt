cmake_minimum_required(VERSION 3.28)
include("../../CMakeCommons.cmake")
file(GLOB_RECURSE ksaieng_SRC CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library(ksaieng STATIC ${ksaieng_SRC})

PrecompileStdHeaders(ksaieng)







find_package(FFMPEG REQUIRED)
target_include_directories(ksaieng PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(ksaieng PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(ksaieng PRIVATE ${FFMPEG_LIBRARIES})

target_link_libraries(ksaieng PRIVATE ksaicore)
