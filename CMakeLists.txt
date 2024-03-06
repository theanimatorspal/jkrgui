include("CMakeCommon.cmake")
cmake_minimum_required(VERSION 3.27)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# If building for Android, set specific compiler flags
if(ANDROID)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -Wl,--exclude-libs,ALL")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
endif()


# Define debug flags for Android builds
if(ANDROID)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(-D_DEBUG)
        set(NDK_DEBUG true)
    endif()
endif()

set(VULKAN_PATH "${VULKAN_SDK}/${VULKAN_VERSION}")

# Set include and library paths based on the platform
if(APPLE)
    set(VULKAN_INCLUDE_PATH "${VULKAN_SDK}/${VULKAN_VERSION}/macOS/include/")
    set(VULKAN_LIBRARY_PATH "${VULKAN_SDK}/${VULKAN_VERSION}/macOS/lib/")
else()
    set(VULKAN_INCLUDE_PATH "${VULKAN_SDK}/${VULKAN_VERSION}/Include/")
    set(VULKAN_LIBRARY_PATH "${VULKAN_SDK}/${VULKAN_VERSION}/Lib/")
endif()

# Set the number of parallel builds
set(CMAKE_BUILD_PARALLEL_LEVEL 8)

# Define additional flags based on the platform and build type
if(ANDROID OR APPLE)
    #add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SDL_MAIN_HANDLED)
else()
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SDL_MAIN_HANDLED -DTRACY_ENABLE)
    else()
        add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SDL_MAIN_HANDLED)
    endif()
endif()


# Set debug information format for MSVC
if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Add platform-specific definitions
if(ANDROID)
    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)
    add_definitions(-DANDROID)
    add_definitions(-DVULKAN_HPP_NO_SMART_HANDLE)
    set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
    include_directories(${APP_GLUE_DIR})
elseif(WIN32)
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)
endif()

# Project settings
project("jkrengine")

# Include directories
include_directories(application)
include_directories(jkrgui)
include_directories(jkrgui/Renderers/ThreeD)
include_directories(jkrgui/Renderers/TwoD)
include_directories(jkrgui/Renderers/TextD)
include_directories(ksaivulkan)
include_directories(ksaivulkan/Vulkan)
include_directories(ksaivulkan/Vendor)
include_directories(ksaivulkan/Vendor/stbi)
include_directories(${SPIRV-Headers_SOURCE_DIR}/include})
include_directories("${CMAKE_SOURCE_DIR}/libs/")
include_directories(aJkrLua)
include_directories(vendor/lua)

# Additional include directories based on platform
if(APPLE)
elseif(ANDROID)
    include_directories("${CMAKE_SOURCE_DIR}/libs/AndroidInclude/SDL2")
else()
    include_directories(ksaivulkan/Vendor/Tracy/tracy)
endif()

# Find Vulkan package
message(STATUS "Using module to find Vulkan")
find_package(Vulkan REQUIRED)

# Output information about Vulkan SDK
message(STATUS "Attempting to locate Vulkan SDK using manual path......")
message(STATUS "Using manual specified path: ${VULKAN_PATH}")
message(STATUS "Vulkan Library Directory ${Vulkan_LIBRARIES}")
include_directories(${VULKAN_INCLUDE_PATH})

# Link directories based on platform
if(ANDROID)
    link_directories("${CMAKE_SOURCE_DIR}/libs/Android/")
else()
    link_directories(${VULKAN_LIBRARY_PATH})
    link_directories("${CMAKE_SOURCE_DIR}/libs/")
endif()

# Additional link directories for Apple
if(APPLE)
    link_directories("/opt/homebrew/lib/")
    link_directories("/usr/local/lib/")
endif()

# Add subdirectories for building
add_subdirectory(vendor/lua)
add_subdirectory(ksaivulkan)
add_subdirectory(aJkrLua)
add_subdirectory(jkrgui)
add_subdirectory("JkrLuaExe")
add_subdirectory("ksai_problems")

# Add unit testing subdirectory if not building for Android or Apple
if(NOT ANDROID AND NOT APPLE)
    add_subdirectory(aa_unit_testing)
endif()

add_custom_target(post_build_custom_target)
copyCompileCommands(post_build_custom_target)