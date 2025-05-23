set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

if(NOT WIN32)
    set(BUILD_SHARED_LIBS FALSE)
else()
    set(BUILD_SHARED_LIBS TRUE)
endif()

set(BUILD_SHARED_LIBS FALSE)

set(CMAKE_UNITY_BUILD_BATCH_SIZE 1000)
# set(CMAKE_UNITY_BUILD ON)

if(ANDROID)
set(BUILD_SHARED_LIBS FALSE)
endif()

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)     # Show download progress
set(FETCHCONTENT_FULLY_DISCONNECTED OFF)  # Ensure re-download if needed
# the number of parallel builds
set(CMAKE_BUILD_PARALLEL_LEVEL 8)
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
# Define additional flags based on the platform and build type
if(ANDROID)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DLUA_USE_LINUX)
elseif(APPLE)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SDL_MAIN_HANDLED)
else()
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SDL_MAIN_HANDLED)
    else()
        add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SDL_MAIN_HANDLED)
    endif()
endif()

# Set debug information format for MSVC
if(MSVC)
    if(POLICY CMP0141)
        cmake_policy(SET CMP0141 NEW)
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
    endif()
endif()
# SET C++ STANDARD TO C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0069 NEW) 
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX)
endif()

set(FETCH_AND_COMP OFF)

# if(ANDROID)
#     set(FETCH_AND_COMP ON)
# endif()

if(FETCH_AND_COMP)
    # Fetch SDL2
    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.30.8       # Use the specific tag for version 2.30.8
        GIT_SHALLOW TRUE             # Fetch only the latest commit
        GIT_PROGRESS TRUE
    )

    # set(SDL_VIDEO ON CACHE BOOL "" FORCE)       # Keep video but disable renderers
    # set(SDL_RENDER OFF CACHE BOOL "" FORCE)     # Disable SDL's software/hardware renderer
    # set(SDL_OPENGL OFF CACHE BOOL "" FORCE)     # Disable OpenGL support
    # set(SDL_METAL OFF CACHE BOOL "" FORCE)      # Disable Metal support
    # set(SDL_VULKAN ON CACHE BOOL "" FORCE)      # Keep Vulkan support enabled
    # set(SDL_SHARED ON CACHE BOOL "" FORCE)
    # set(SDL_STATIC OFF CACHE BOOL "" FORCE)
    # Make the content available
    FetchContent_MakeAvailable(SDL2)

    # Fetch SPIRV-Headers
    FetchContent_Declare(
        spirv-headers
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Headers.git
        GIT_TAG vulkan-sdk-1.4.304.0     
        GIT_SHALLOW TRUE          
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(spirv-headers)

    # Fetch SPIRV-Tools
    set(SPIRV_TOOLS_BUILD_STATIC ON CACHE BOOL "Build SPIRV-Tools as a static library" FORCE)
    set(SPIRV_SKIP_TESTS ON CACHE BOOL "Disable tests in SPIRV-Tools" FORCE)
    set(SPIRV_SKIP_EXECUTABLES ON CACHE BOOL "Disable executables in SPIRV-Tools" FORCE)
    FetchContent_Declare(
        spirv-tools
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Tools.git
        GIT_TAG vulkan-sdk-1.4.304.0     
        GIT_SHALLOW TRUE          
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(spirv-tools)

    set(SPIRV_CROSS_SKIP_EXECUTABLES ON CACHE BOOL "Disable SPIRV-Cross executables" FORCE)
    FetchContent_Declare(
        spirv-cross
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git
        GIT_TAG vulkan-sdk-1.4.304.0     
        GIT_SHALLOW TRUE          
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(spirv-cross)

    # Fetch glslang
    set(ENABLE_SPVREMAPPER ON CACHE BOOL "Disable building spirv-remap tool" FORCE)
    set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "Disable building glslangValidator and other binaries" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "Disable testing in glslang" FORCE)
    FetchContent_Declare(
        glslang
        GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
        GIT_TAG vulkan-sdk-1.4.304.0  
        GIT_SHALLOW TRUE            
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(glslang)

    set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
    set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)  # Ensure static build
    # Fetch Freetype
    FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
        GIT_TAG VER-2-13-2
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(freetype)


    set(HARFBUZZ_BUILD_STANDALONE OFF CACHE BOOL "Build Harfbuzz as a dependency" FORCE)
    set(HARFBUZZ_USE_ICU OFF)
    # Fetch Harfbuzz
    FetchContent_Declare(
        harfbuzz
        GIT_REPOSITORY https://github.com/harfbuzz/harfbuzz.git
        GIT_TAG 8.2.2
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(harfbuzz)
    if(WIN32)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
            target_link_libraries(freetype msvcrtd)
            target_link_libraries(glslang msvcrtd)
        endif()
    endif()
endif()


set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
# Include directories
include_directories(${CMAKE_JKRGUI_DIR}/application)
include_directories(${CMAKE_JKRGUI_DIR}/jkrengine)
include_directories(${CMAKE_JKRGUI_DIR}/ksaivulkan)
include_directories(${CMAKE_JKRGUI_DIR}/ksaivulkan/Vulkan)
include_directories(${CMAKE_JKRGUI_DIR}/ksaivulkan/Vendor)
include_directories(${CMAKE_JKRGUI_DIR}/ksaivulkan/Vendor/stbi)
include_directories(${CMAKE_JKRGUI_DIR}/ksaivulkan/Vendor/asio)
include_directories(${CMAKE_JKRGUI_DIR}/libs/)
include_directories(${CMAKE_JKRGUI_DIR}/vendor/lua)

function(PrecompileStdHeaders TARGET_NAME)
    target_precompile_headers(${TARGET_NAME} PRIVATE
        # Standard C++ Headers
        <algorithm>
        <any>
        <array>
        <atomic>
        <bit>
        <bitset>
        <cassert>
        <cctype>
        <cerrno>
        <cfenv>
        <charconv>
        <chrono>
        <cinttypes>
        <climits>
        <clocale>
        <cmath>
        <codecvt>
        <compare>
        <complex>
        <concepts>
        <condition_variable>
        <coroutine>
        <csetjmp>
        <csignal>
        <cstdarg>
        <cstddef>
        <cstdint>
        <cstdio>
        <cstdlib>
        <cstring>
        <ctime>
        <deque>
        <exception>
        <execution>
        <expected>
        <filesystem>
        <format>
        <forward_list>
        <fstream>
        <functional>
        <future>
        <initializer_list>
        <iomanip>
        <ios>
        <iosfwd>
        <iostream>
        <istream>
        <iterator>
        <limits>
        <list>
        <locale>
        <map>
        <memory>
        <memory_resource>
        <mutex>
        <new>
        <numbers>
        <numeric>
        <optional>
        <ostream>
        <queue>
        <random>
        <ranges>
        <ratio>
        <regex>
        <scoped_allocator>
        <set>
        <shared_mutex>
        <source_location>
        <span>
        <sstream>
        <stack>
        <stdexcept>
        <streambuf>
        <string>
        <string_view>
        <system_error>
        <thread>
        <tuple>
        <type_traits>
        <typeindex>
        <typeinfo>
        <unordered_map>
        <unordered_set>
        <utility>
        <valarray>
        <variant>
        <vector>
        <version>

        # C Standard Library Headers (as C++ headers)
        <cctype>
        <cerrno>
        <cfenv>
        <cfloat>
        <climits>
        <clocale>
        <cmath>
        <csetjmp>
        <csignal>
        <cstdarg>
        <cstddef>
        <cstdint>
        <cstdio>
        <cstdlib>
        <cstring>
        <ctime>
        <cwchar>
        <cwctype>

        # Third-Party Headers (Example: Vulkan, GLM, Sol2)
    #     <sol/sol.hpp>
    #     <vulkan/vulkan.hpp>
    #     <glm/glm.hpp>
    #     <glm/fwd.hpp>
    #     <glm/gtc/matrix_transform.hpp>
    #     <glm/gtx/matrix_decompose.hpp>
    #     <glm/gtc/type_ptr.hpp>
    #     <glm/gtx/quaternion.hpp>
    #     <glm/ext.hpp>
    #     <glm/gtx/transform.hpp>
    )
endfunction()


# If building for Android, set specific compiler flags
# Define debug flags for Android builds
if(ANDROID)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -Wl,--exclude-libs,ALL")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
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

# Set 

# Add platform-specific definitions
if(ANDROID)
    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)
    add_definitions(-DANDROID)
    add_definitions(-DVULKAN_HPP_NO_SMART_HANDLE)
    set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
    include_directories(${APP_GLUE_DIR})
elseif(WIN32)
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)
elseif(APPLE)
    add_definitions(-DLUA_USE_MACOSX)
endif()

if(APPLE)
elseif(ANDROID)
    include_directories("${CMAKE_JKRGUI_DIR}/libs/AndroidInclude/SDL2")
    include_directories(${JNI_INCLUDE_DIRS})
else()
    include_directories(ksaivulkan/Vendor/Tracy/tracy)
endif()

if(WIN32)
    set(Vulkan_LIBRARIES "vulkan-1")
else()
    set(Vulkan_LIBRARIES "vulkan")
endif()


include_directories(${VULKAN_INCLUDE_PATH})
# Link directories based on platform
if(ANDROID)
link_directories("${CMAKE_JKRGUI_DIR}/libs/Android/${CMAKE_ANDROID_ARCH_ABI}/")
else()
    link_directories(${VULKAN_LIBRARY_PATH})
    link_directories("${CMAKE_JKRGUI_DIR}/libs/")
endif()

# Additional link directories for Apple
if(APPLE)
    link_directories("/opt/homebrew/lib/")
    link_directories("/usr/local/lib/")
endif()

add_definitions(-DWIN32_LEAN_AND_MEAN)

# Define a function to set up the target based on the build type
if(APPLE)
function(configure_target TARGET_NAME)
        target_link_libraries(${TARGET_NAME}
            ${Vulkan_LIBRARIES}
            SDL2
            SPIRV
            MachineIndependent
            SPIRV-Tools
            SPIRV-Tools-diff
            SPIRV-Tools-link
            SPIRV-Tools-lint
            SPIRV-Tools-opt
            SPIRV-Tools-reduce
            SPIRV-Tools-shared
            OSDependent
            GenericCodeGen
            glslang-default-resource-limits
            glslang
            spirv-cross-c-shared
            spirv-cross-c
            spirv-cross-core
            spirv-cross-cpp
            spirv-cross-glsl
            spirv-cross-hlsl
            spirv-cross-msl
            spirv-cross-reflect
            lua
            freetype
            harfbuzz
            harfbuzz-subset
        )
endfunction()

elseif(ANDROID)

function(configure_target TARGET_NAME)
   target_link_libraries(${TARGET_NAME}
        vulkan # IDK Why ${Vulkan_LIBRARIES are not working}
        android
        log
        # jnigraphics
        brotlicommon-static
        brotlidec-static
        brotlienc-static
        bz2

        freetype
        GenericCodeGen
        glslang-default-resource-limits
        glslang
        harfbuzz-subset
        harfbuzz
        lua
        MachineIndependent
        OSDependent

        png
        png16

        SDL2
        spirv-cross-c
        spirv-cross-core
        spirv-cross-cpp
        spirv-cross-glsl
        spirv-cross-hlsl
        spirv-cross-msl
        spirv-cross-reflect
        spirv-cross-util
        SPIRV-Tools-diff
        SPIRV-Tools-link
        SPIRV-Tools-lint
        SPIRV-Tools-opt
        SPIRV-Tools-reduce
        SPIRV-Tools
        SPIRV
        SPVRemapper
        z
   )
endfunction()


else()


function(configure_target TARGET_NAME)
    if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        target_link_libraries(${TARGET_NAME}
            ${Vulkan_LIBRARIES}
            SDL2
            SPIRV
            MachineIndependent
            SPIRV-Tools
            SPIRV-Tools-diff
            SPIRV-Tools-link
            SPIRV-Tools-lint
            SPIRV-Tools-opt
            SPIRV-Tools-reduce
            SPIRV-Tools-shared
            OSDependent
            GenericCodeGen
            glslang-default-resource-limits
            glslang
            spirv-cross-c-shared
            spirv-cross-c
            spirv-cross-core
            spirv-cross-cpp
            spirv-cross-glsl
            spirv-cross-hlsl
            spirv-cross-msl
            spirv-cross-reflect
            lua
            freetype
            harfbuzz
            harfbuzz-subset
            user32
            ws2_32
            wsock32
            dbghelp
            ntdll
            advapi32
        )
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(${TARGET_NAME}
            ${Vulkan_LIBRARIES}
            SDL2
            SPIRVd
            MachineIndependentd
            SPIRV-Toolsd
            SPIRV-Tools-diffd
            SPIRV-Tools-linkd
            SPIRV-Tools-lintd
            SPIRV-Tools-optd
            SPIRV-Tools-reduced
            SPIRV-Tools-sharedd
            OSDependentd
            GenericCodeGend
            glslang-default-resource-limitsd
            glslangd
            spirv-cross-c-sharedd
            spirv-cross-cd
            spirv-cross-cored
            spirv-cross-cppd
            spirv-cross-glsld
            spirv-cross-hlsld
            spirv-cross-msld
            spirv-cross-reflectd
            lua
            freetype
            harfbuzz
            harfbuzz-subset
            user32
            ws2_32
            wsock32
            dbghelp
            ntdll
            advapi32
        )
    endif()
endfunction()

endif()

function(someFlags inName)
     set_property(TARGET ${inName} PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
endfunction()