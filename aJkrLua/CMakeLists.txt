include("../CMakeCommon.cmake")


file(GLOB app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
)

if(ANDROID)
add_library(aJkrLua SHARED ${app_SRC})
else()
add_executable(aJkrLua ${app_SRC})
endif()

#if(MSVC) 
#    target_link_options(aJkrLua PRIVATE 
#        # $<$<CONFIG:Debug>:/INCREMENTAL> is active by default for debug
#        $<$<CONFIG:Release>:/INCREMENTAL:NO>
#        $<$<CONFIG:RelWithDebInfo>:/INCREMENTAL:NO>
#    )
#endif()

target_link_libraries(aJkrLua ksaivulkan jkrgui)
configure_target(aJkrLua)

someFlags(ksaivulkan)
COPYTODESTINATION(aJkrLua)
