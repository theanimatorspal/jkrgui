set(PLAYGROUND_SRC
"main.cpp"
)

add_executable(playground ${PLAYGROUND_SRC})

PrecompileStdHeaders(playground)
# target_link_libraries(playground ksaivulkan jkrgui)
configure_target(playground)


add_custom_command(
    TARGET playground POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_JKRGUI_DIR}/out/build/${CMAKE_PRESET_NAME}/compile_commands.json"
            "${CMAKE_JKRGUI_DIR}/compile_commands.json"
)

set(PLAYGROUND2_SRC
"main2.cpp"
)

add_executable(playground1 ${PLAYGROUND2_SRC})