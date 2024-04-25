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

PrecompileStdHeaders(JkrLuaExe)
target_link_libraries(JkrLuaExe ksaivulkan jkrgui)
configure_target(JkrLuaExe)

set(SrcDirectory "${CMAKE_JKRGUI_DIR}/out/build/${CMAKE_PRESET_NAME}")
set(DebugDirectory "${CMAKE_JKRGUI_DIR}/out/build/x64-debug/JkrLuaExe")

message(STATUS "Copying from Src to DebugDirectory")
message(STATUS "${SrcDirectory} to ${DebugDirectory}")

# if(CMAKE_BUILD_TYPE STREQUAL "Release")
#     add_custom_command(
#         TARGET JkrLuaExe POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy
#                 $<TARGET_FILE:JkrLuaExe>
#                 ${DebugDirectory}/
#     )
# endif()

add_custom_command(
    TARGET JkrLuaExe POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_JKRGUI_DIR}/out/build/${CMAKE_PRESET_NAME}/compile_commands.json"
            "${CMAKE_JKRGUI_DIR}/compile_commands.json"
)