cmake_minimum_required(VERSION 3.27)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set(BUILD_SHARED_LIBS TRUE)
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
# Add subdirectories for building
add_subdirectory(vendor/lua)
add_subdirectory(ksaivulkan)
add_subdirectory(jkrengine)
add_subdirectory(jkrgui)
# add_subdirectory(playground)

export(TARGETS lua FILE luaExport.cmake)
export(TARGETS ksaivulkan FILE ksaivulkanExport.cmake)
export(TARGETS jkrengine FILE jkrengineExport.cmake)
message(STATUS "CMAKE Flags:::" ${CMAKE_CXX_FLAGS})