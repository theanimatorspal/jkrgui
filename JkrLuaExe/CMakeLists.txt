add_executable(JkrLuaExe 
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
     )
target_link_libraries(JkrLuaExe ksaivulkan jkrgui)
configure_target(JkrLuaExe)