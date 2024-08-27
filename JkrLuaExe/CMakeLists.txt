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

if(ANDROID)
     add_library(JkrLuaExe SHARED ${JKR_SOURCES})
     set_target_properties(JkrLuaExe PROPERTIES LINK_FLAGS "-Wl,--export-dynamic-symbol=main")
else()
     add_executable(JkrLuaExe ${JKR_SOURCES})
endif()

PrecompileStdHeaders(JkrLuaExe)
target_link_libraries(JkrLuaExe ksaivulkan jkrgui)
configure_target(JkrLuaExe)

add_custom_command(
    TARGET JkrLuaExe POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_JKRGUI_DIR}/out/build/${CMAKE_PRESET_NAME}/compile_commands.json"
            "${CMAKE_JKRGUI_DIR}/compile_commands.json"
)