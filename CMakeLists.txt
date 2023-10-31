# CMakeList.txt : CMake project for jkrengine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(VULKAN_SDK "C:/VulkanSDK")
set(VULKAN_VERSION "1.3.250.1")
set(VULKAN_PATH "${VULKAN_SDK}/${VULKAN_VERSION}")
set(VULKAN_INCLUDE_PATH "${VULKAN_SDK}/${VULKAN_VERSION}/Include/")
set(VULKAN_LIBRARY_PATH "${VULKAN_SDK}/${VULKAN_VERSION}/Lib/")
set(CMAKE_BUILD_PARALLEL_LEVEL 8)

add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SDL_MAIN_HANDLED)

set(CMAKE_CXX_STANDARD 20)


project ("jkrengine")

include_directories(application)
include_directories(jkrgui)
include_directories(ksaivulkan)
include_directories(ksaivulkan/Vulkan)
include_directories(ksaivulkan/Vendor)
include_directories(ksaivulkan/Vendor/lua)

message(STATUS "Using module to find Vulkan")
find_package(Vulkan REQUIRED)

message(STATUS "Attempting to locate Vulkan SDK using manual path......")
message(STATUS "Using manual specified path: ${VULKAN_PATH}")
message(STATUS "Vulkan Library Directory ${Vulkan_LIBRARIES}")
include_directories(${VULKAN_INCLUDE_PATH})

link_directories(${VULKAN_LIBRARY_PATH})
link_directories("${CMAKE_SOURCE_DIR}/libs/")

add_subdirectory(application)
add_subdirectory(jkrgui)
add_subdirectory(ksaivulkan)




