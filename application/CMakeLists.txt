file(GLOB app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
)

link_directories(${VULKAN_LIBRARY_PATH})

add_executable(application ${app_SRC} "application.cpp" "DrawArea.hpp" "DrawArea.cpp" "GridSheet.cpp")

include("../CMakeCommon.cmake")

configure_target(application)

