add_executable(JkrLuaExe "JkrLuaExe.cpp" "JkrLuaExe.hpp" "JkrMath.cpp" "JkrKeys.cpp")
target_link_libraries(JkrLuaExe ksaivulkan jkrgui)
configure_target(JkrLuaExe)
