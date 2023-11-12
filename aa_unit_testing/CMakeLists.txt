file(GLOB app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
)

link_directories(${VULKAN_LIBRARY_PATH})


add_executable(aa_unit_testing ${app_SRC} "testmain.cpp")

include("../CMakeCommon.txt")

configure_target(aa_unit_testing)

