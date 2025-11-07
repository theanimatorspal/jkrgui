cmake_minimum_required(VERSION 3.28)
include("../../CMakeCommons.cmake")
file(GLOB_RECURSE jkrgui_SRC CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_executable(jkrgui  ${jkrgui_SRC})

PrecompileStdHeaders(jkrgui)

find_package(Lua REQUIRED)
target_include_directories(jkrgui PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(jkrgui PRIVATE ${LUA_LIBRARIES})


find_path(SOL2_INCLUDE_DIRS "sol/abort.hpp")
target_include_directories(jkrgui PRIVATE ${SOL2_INCLUDE_DIRS})

target_link_libraries(jkrgui PRIVATE ksaicore PRIVATE ksaieng)
add_custom_command(
    TARGET jkrgui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/out/build/${CMAKE_PRESET_NAME}/compile_commands.json"
            "${CMAKE_SOURCE_DIR}/compile_commands.json"
)
