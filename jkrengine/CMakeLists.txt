include("../CMakeCommon.cmake")

file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
)

if(ANDROID)
	add_library(jkrengine STATIC ${app_SRC})
	 target_link_libraries(jkrengine ksaivulkan)
	 configure_target(jkrengine)
else()
	add_library(jkrengine ${app_SRC})
	target_link_libraries(jkrengine ksaivulkan)
	configure_target(jkrengine)
endif()

PrecompileStdHeaders(jkrengine)
