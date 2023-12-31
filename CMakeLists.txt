﻿# CMakeList.txt : CMake project for jkrengine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

if(APPLE)
	set(VULKAN_SDK "/Users/bishaljaiswal/VulkanSDK")
	set(VULKAN_VERSION "1.3.268.1")
else(WIN32)
	set(VULKAN_SDK "C:/VulkanSDK")
	set(VULKAN_VERSION "1.3.250.1")
	set(LLVM_PATH "C:/Users/sansk/OneDrive/Documents/llvm-project-llvmorg-17.0.6/llvm-project-llvmorg-17.0.6/llvm")
endif()

set(VULKAN_PATH "${VULKAN_SDK}/${VULKAN_VERSION}")


if(APPLE)
        set(VULKAN_INCLUDE_PATH "${VULKAN_SDK}/${VULKAN_VERSION}/macOS/include/")
        set(VULKAN_LIBRARY_PATH "${VULKAN_SDK}/${VULKAN_VERSION}/macOS/lib/")
else()
        set(VULKAN_INCLUDE_PATH "${VULKAN_SDK}/${VULKAN_VERSION}/Include/")
        set(VULKAN_LIBRARY_PATH "${VULKAN_SDK}/${VULKAN_VERSION}/Lib/")
endif()

set(CMAKE_BUILD_PARALLEL_LEVEL 8)

add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SDL_MAIN_HANDLED -DTRACY_ENABLE)

set(CMAKE_CXX_STANDARD 20)



if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


if (ANDROID)
    add_definitions(-DVULKAN_HPP_NO_SMART_HANDLE)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
    set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
    include_directories(${APP_GLUE_DIR})
    add_library(app-glue STATIC ${APP_GLUE_DIR}/android_native_app_glue.c)
endif()

if (ANDROID)
    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)
elseif (WIN32)
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)
    # add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
    # find_package(XCB REQUIRED)
    # link_libraries(${XCB_LIBRARIES})
endif()


project ("jkrengine")

include_directories(application)
include_directories(jkrgui)
include_directories(jkrgui/Renderers/ThreeD)
include_directories(jkrgui/Renderers/TwoD)
include_directories(jkrgui/Renderers/TextD)

include_directories(ksaivulkan)
include_directories(ksaivulkan/Vulkan)
include_directories(ksaivulkan/Vendor)
include_directories(ksaivulkan/Vendor/stbi)
include_directories(ksaivulkan/Vendor/lua)
include_directories("${CMAKE_SOURCE_DIR}/libs/")
if(APPLE)
else()
include_directories("${LLVM_PATH}/include")
endif()


message(STATUS "Using module to find Vulkan")
find_package(Vulkan REQUIRED)

message(STATUS "Attempting to locate Vulkan SDK using manual path......")
message(STATUS "Using manual specified path: ${VULKAN_PATH}")
message(STATUS "Vulkan Library Directory ${Vulkan_LIBRARIES}")
include_directories(${VULKAN_INCLUDE_PATH})

link_directories(${VULKAN_LIBRARY_PATH})
link_directories("${CMAKE_SOURCE_DIR}/libs/")

if(APPLE)
	link_directories("/opt/homebrew/lib/")
	link_directories("/usr/local/lib/")
else()
	if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
		   link_directories("${LLVM_PATH}/Release/lib")
	else()
		   link_directories("${LLVM_PATH}/Debug/lib")
	endif()
endif()

add_subdirectory(aa_unit_testing)
add_subdirectory(jkrgui)
add_subdirectory(ksaivulkan)
add_subdirectory(aJkrLua)

if(APPLE)
else()
add_subdirectory(jit)
endif()
