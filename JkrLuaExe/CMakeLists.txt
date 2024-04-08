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

target_link_libraries(JkrLuaExe ksaivulkan jkrgui)
configure_target(JkrLuaExe)