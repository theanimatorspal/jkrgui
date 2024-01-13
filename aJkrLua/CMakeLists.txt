include("../CMakeCommon.cmake")

file(GLOB app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
)

link_directories(${VULKAN_LIBRARY_PATH})

add_executable(aJkrLua ${app_SRC} "EntryPoint.cpp" "Bindings.cpp" "Bindings3d.cpp")

configure_target(aJkrLua)
