include("../CMakeCommon.cmake")

file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
)

if(ANDROID)
	add_library(jkrgui STATIC ${app_SRC})
	 target_link_libraries(jkrgui ksaivulkan)
	 configure_target(jkrgui)
else()
	add_library(jkrgui STATIC ${app_SRC})
 endif()

 someFlags(jkrgui)
