include("../CMakeCommon.cmake")

file(GLOB app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
)

link_directories(${VULKAN_LIBRARY_PATH})


add_executable(aa_unit_testing ${app_SRC} "testmain.cpp" "nodetest.cpp" "threed.cpp" "fontagina.cpp" "Neural.cpp")


configure_target(aa_unit_testing)
target_link_libraries(aa_unit_testing ksaivulkan jkrgui)

