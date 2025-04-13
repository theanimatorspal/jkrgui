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
    "JkrAudio.cpp"
    "JkrTextRenderers.cpp"
    "JkrMultiThreading.cpp"
    "JkrLuaPlatform.cpp"
    "JkrNetwork.cpp"
)

# if(WIN32)
#     list(APPEND JKR_SOURCES "../ksaivulkan/Vendor/Tracy/TracyClient.cpp")
# endif()

if(ANDROID)
     add_library(jkrgui SHARED ${JKR_SOURCES})
     set_target_properties(jkrgui PROPERTIES LINK_FLAGS "-Wl,--export-dynamic-symbol=main")
else()
     add_executable(jkrgui ${JKR_SOURCES})
endif()

set_target_properties(jkrgui PROPERTIES OUTPUT_NAME "jkrgui")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
if(NOT ANDROID)
     set_target_properties(jkrgui PROPERTIES OUTPUT_NAME "jkrgui-r")
endif()
endif()


PrecompileStdHeaders(jkrgui)
target_link_libraries(jkrgui ksaivulkan jkrengine)
configure_target(jkrgui)

add_custom_command(
    TARGET jkrgui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_JKRGUI_DIR}/out/build/${CMAKE_PRESET_NAME}/compile_commands.json"
            "${CMAKE_JKRGUI_DIR}/compile_commands.json"
)