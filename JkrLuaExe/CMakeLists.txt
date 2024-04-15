set(JKR_SOURCES
    "JkrLuaExe.cpp"
    "JkrLuaExe.hpp"
    "JkrMath.cpp"
    "JkrKeys.cpp" 
    "JkrDevEnv.cpp" 
    "JkrBuildSystem.cpp" 
    "JkrMisc.cpp"
    "JkrRenderer.cpp"
    "JkrRenderer3D.cpp"
    "JkrBasic.cpp"
    "JkrTextRenderers.cpp"
    "JkrMultiThreading.cpp"
)
if(ANDROID)
     add_library(JkrLuaExe SHARED ${JKR_SOURCES})
else()
     add_executable(JkrLuaExe ${JKR_SOURCES})
endif()

target_precompile_headers ( JkrLuaExe
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
)

target_link_libraries(JkrLuaExe ksaivulkan jkrgui)
configure_target(JkrLuaExe)

set(SrcDirectory "${CMAKE_JKRGUI_DIR}/out/build/${CMAKE_PRESET_NAME}")
set(DebugDirectory "${CMAKE_JKRGUI_DIR}/out/build/x64-debug/JkrLuaExe")

message(STATUS "Copying from Src to DebugDirectory")
message(STATUS "${SrcDirectory} to ${DebugDirectory}")

add_custom_command(
    TARGET JkrLuaExe POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:JkrLuaExe>
            ${DebugDirectory}/
)