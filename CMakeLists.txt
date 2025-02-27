cmake_minimum_required(VERSION 3.27)
include("CMakeConfig.cmake")
include("CMakeCommon.cmake")




project("jkrengine")
if(ANDROID)
find_package(JNI REQUIRED)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_JKRGUI_DIR}/out/build/${CMAKE_PRESET_NAME}/jkrgui/")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_JKRGUI_DIR}/out/build/${CMAKE_PRESET_NAME}/jkrgui/")
endif()

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-Wno-nullability-completeness" COMPILER_SUPPORTS_NULLABILITY)
if (COMPILER_SUPPORTS_NULLABILITY)
    add_compile_options(-Wno-nullability-completeness)
endif()

add_subdirectory(vendor/lua)
add_subdirectory(ksaivulkan)
add_subdirectory(jkrengine)
add_subdirectory(jkrgui)

message(STATUS "CMAKE Flags:::" ${CMAKE_CXX_FLAGS})