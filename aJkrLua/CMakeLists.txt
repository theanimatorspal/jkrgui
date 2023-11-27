include("../CMakeCommon.cmake")

file(GLOB app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
)

link_directories(${VULKAN_LIBRARY_PATH})

add_executable(aJkrLua ${app_SRC} "EntryPoint.cpp")

configure_target(aJkrLua)
