include("CMakeCommon.cmake")

cmake_minimum_required (VERSION 3.27)

if(ANDROID)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -Wl,--exclude-libs,ALL")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s") 
endif()

#set your VULKAN_SDK PATH 
if(APPLE)
	set(VULKAN_SDK "/Users/bishaljaiswal/VulkanSDK")
	set(VULKAN_VERSION "1.3.268.1")
elseif(WIN32)
	set(VULKAN_SDK "C:/VulkanSDK")
	set(VULKAN_VERSION "1.3.250.1")
endif()

if(ANDROID)
     if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_definitions(-D_DEBUG)
		set(NDK_DEBUG true)
	endif()
endif()

#set(ANDROID_NDK "C:/Users/sansk/AppData/Local/Android/Sdk/ndk/25.1.8937393")

set(VULKAN_PATH "${VULKAN_SDK}/${VULKAN_VERSION}")


if(APPLE)
        set(VULKAN_INCLUDE_PATH "${VULKAN_SDK}/${VULKAN_VERSION}/macOS/include/")
        set(VULKAN_LIBRARY_PATH "${VULKAN_SDK}/${VULKAN_VERSION}/macOS/lib/")
else()
        set(VULKAN_INCLUDE_PATH "${VULKAN_SDK}/${VULKAN_VERSION}/Include/")
        set(VULKAN_LIBRARY_PATH "${VULKAN_SDK}/${VULKAN_VERSION}/Lib/")
endif()

set(CMAKE_BUILD_PARALLEL_LEVEL 8)

if(ANDROID OR APPLE)
#add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SDL_MAIN_HANDLED)
else()
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SDL_MAIN_HANDLED -DTRACY_ENABLE)
    else()
        add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SDL_MAIN_HANDLED)
    endif()
endif()

set(CMAKE_CXX_STANDARD 20)



if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if (ANDROID)
    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)
    add_definitions(-DANDROID)
    add_definitions(-DVULKAN_HPP_NO_SMART_HANDLE)
    set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
    include_directories(${APP_GLUE_DIR})
    #add_library(app-glue STATIC ${APP_GLUE_DIR}/android_native_app_glue.c)
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
include_directories(lua/)
include_directories("${CMAKE_SOURCE_DIR}/libs/")

include_directories(aJkrLua)

if(APPLE)
elseif(ANDROID)
include_directories("${CMAKE_SOURCE_DIR}/libs/AndroidInclude/SDL2")
else()
include_directories(ksaivulkan/Vendor/Tracy/tracy)
endif()


message(STATUS "Using module to find Vulkan")
find_package(Vulkan REQUIRED)

message(STATUS "Attempting to locate Vulkan SDK using manual path......")
message(STATUS "Using manual specified path: ${VULKAN_PATH}")
message(STATUS "Vulkan Library Directory ${Vulkan_LIBRARIES}")
include_directories(${VULKAN_INCLUDE_PATH})

if(ANDROID)
	link_directories("${CMAKE_SOURCE_DIR}/libs/Android/")
else()
	link_directories(${VULKAN_LIBRARY_PATH})
	link_directories("${CMAKE_SOURCE_DIR}/libs/")
endif()

if(APPLE)
	link_directories("/opt/homebrew/lib/")
	link_directories("/usr/local/lib/")
endif()

add_subdirectory(ksaivulkan)
add_subdirectory(aJkrLua)
add_subdirectory(jkrgui)

if(APPLE)
elseif(ANDROID)
else()
add_subdirectory(aa_unit_testing)
endif()

add_subdirectory("JkrLuaExe")
add_subdirectory("lua")
