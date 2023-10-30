file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
)

link_directories(${VULKAN_LIBRARY_PATH})

add_executable(application ${app_SRC})


target_link_libraries(application 
	${Vulkan_LIBRARIES}
	ksaivulkan 
	jkrgui 
	SDL2
)
