cmake_minimum_required(VERSION 3.27)

include("CMakeConfig.cmake")
include("CMakeCommon.cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project("jkrengine")
find_package(JNI REQUIRED)

# Add subdirectories for building
add_subdirectory(vendor/lua)
add_subdirectory(ksaivulkan)
add_subdirectory(jkrgui)
add_subdirectory(JkrLuaExe)
add_subdirectory(playground)

export(TARGETS lua FILE luaExport.cmake)
export(TARGETS ksaivulkan FILE ksaivulkanExport.cmake)
export(TARGETS jkrgui FILE jkrguiExport.cmake)

message(STATUS "CMAKE Flags:::" ${CMAKE_CXX_FLAGS})
