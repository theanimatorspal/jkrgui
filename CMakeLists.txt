cmake_minimum_required(VERSION 3.27)

include("CMakeConfig.cmake")
include("CMakeCommon.cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
ExcludeSymbolsForAndroid()

project("jkrengine")
# Additional include directories based on platform

# Add subdirectories for building
add_subdirectory(vendor/lua)
add_subdirectory(ksaivulkan)
add_subdirectory(jkrgui)
add_subdirectory(JkrLuaExe)
add_subdirectory(ksai_problems)

export(TARGETS lua FILE luaExport.cmake)
export(TARGETS ksaivulkan FILE ksaivulkanExport.cmake)
export(TARGETS jkrgui FILE jkrguiExport.cmake)

