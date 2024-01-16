include("../CMakeCommon.cmake")

file(GLOB app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
)

if(ANDROID)
add_library(aJkrLua SHARED ${app_SRC} "EntryPoint.cpp" "Bindings.cpp" "Bindings3d.cpp")
else()
add_executable(aJkrLua ${app_SRC} )
endif()

target_link_libraries(aJkrLua ksaivulkan jkrgui)
configure_target(aJkrLua)


if(ANDROID)
add_custom_command(
	TARGET aJkrLua POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	${CMAKE_CURRENT_BINARY_DIR}/libaJkrLua.so
	${CMAKE_SOURCE_DIR}/libs/Android/libaJkrLua.so
)
endif()
