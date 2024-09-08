cmake_minimum_required(VERSION 3.27)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set(BUILD_SHARED_LIBS TRUE)
include("CMakeConfig.cmake")
include("CMakeCommon.cmake")



project("jkrengine")
find_package(JNI REQUIRED)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_JKRGUI_DIR}/out/build/${CMAKE_PRESET_NAME}/jkrgui/")
# Add subdirectories for building
add_subdirectory(vendor/lua)
add_subdirectory(ksaivulkan)
add_subdirectory(jkrengine)
add_subdirectory(jkrgui)
# add_subdirectory(playground)

export(TARGETS lua FILE luaExport.cmake)
export(TARGETS ksaivulkan FILE ksaivulkanExport.cmake)
export(TARGETS jkrgui FILE jkrguiExport.cmake)

message(STATUS "CMAKE Flags:::" ${CMAKE_CXX_FLAGS})
# add_custom_target(post)
# if(WIN32)
#      add_custom_command(
#           TARGET post POST_BUILD
#           COMMAND ${CMAKE_COMMAND} -E copy
#                "${CMAKE_JKRGUI_DIR}/out/build/${CMAKE_PRESET_NAME}/jkrgui/jkrgui.dll"
#                "${CMAKE_JKRGUI_DIR}/out/build/${CMAKE_PRESET_NAME}/JkrLuaExe/jkrgui.dll"
#      )

#      add_custom_command(
#           TARGET post POST_BUILD
#           COMMAND ${CMAKE_COMMAND} -E copy
#                "${CMAKE_JKRGUI_DIR}/out/build/${CMAKE_PRESET_NAME}/ksaivulkan/ksaivulkan.dll"
#                "${CMAKE_JKRGUI_DIR}/out/build/${CMAKE_PRESET_NAME}/JkrLuaExe/ksaivulkan.dll"
#      )
# endif()