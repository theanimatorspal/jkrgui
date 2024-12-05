# SET C++ STANDARD TO C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0069 NEW) 
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX)
endif()

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
target_precompile_headers ( ${TARGET_NAME}
    PRIVATE
        <vector>
        <format>
        <map>
        <unordered_map>
        <array>
        <optional>
        <expected>
        <fstream>
        <filesystem>
        <functional>
        <concepts>
        <memory>
        <string>
        <cstdint>
        <sol/sol.hpp>
        <vulkan/vulkan.hpp>
        <glm/glm.hpp>
        <glm/fwd.hpp>
        <glm/gtc/matrix_transform.hpp>
        <glm/gtx/matrix_decompose.hpp>
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

# Set the number of parallel builds
set(CMAKE_BUILD_PARALLEL_LEVEL 8)
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
# Define additional flags based on the platform and build type
if(ANDROID)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DLUA_USE_LINUX)
elseif(APPLE)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SDL_MAIN_HANDLED)
else()
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SDL_MAIN_HANDLED -DTRACY_ENABLE)
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
    link_directories("${CMAKE_JKRGUI_DIR}/libs/Android/android-${CMAKE_ANDROID_ARCH_ABI}/")
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