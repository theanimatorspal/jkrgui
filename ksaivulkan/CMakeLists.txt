include("../CMakeCommon.cmake")
#if apple then no tracy
set(FILES 
"Vulkan/*.hpp"
"Vulkan/*.cpp"
"Vendor/cglm/*.hpp"
"Vendor/lua/*.hpp"
"Vendor/sol/*.hpp"
"Vendor/stbi/*.hpp"
"Vendor/VulkanMemoryAllocator/*.hpp"
"Vendor/cglm/*.cpp"
"Vendor/lua/*.cpp"
"Vendor/sol/*.cpp"
"Vendor/stbi/*.cpp"
"Vendor/VulkanMemoryAllocator/*.cpp"
"Vendor/cglm/*.h"
"Vendor/lua/*.h"
"Vendor/sol/*.h"
"Vendor/stbi/*.h"
"Vendor/VulkanMemoryAllocator/*.h"
"Vendor/cglm/*.c"
"Vendor/lua/*.c"
"Vendor/sol/*.c"
"Vendor/stbi/*.c"
"Vendor/VulkanMemoryAllocator/*.c"
"Vendor/TinyGLTF/*.cpp"
"Vendor/TinyGLTF/*.hpp"
"Vendor/VulkanMemoryAllocator/vma.cpp"
"Vendor/Eigen/*.*"
"ksaiSandBox/glslHelper.cpp"
"ksaiSandBox/glslHelper.hpp"
"ksai_config.hpp"
"ksai_image.hpp"
"ksai_thread.hpp"
"ksai_config.cpp"
)
if(APPLE OR ANDROID)
        file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS ${FILES})
	   if(ANDROID)
		   add_library(ksaivulkan STATIC ${app_SRC})
	   else()
		   add_library(ksaivulkan ${app_SRC})
	   endif()
          configure_target(ksaivulkan)
else()
        file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS ${FILES}
        "Vendor/Tracy/TracyClient.cpp"
        )
        add_library(ksaivulkan ${app_SRC} "ksai_image.hpp" "ksai_thread.hpp" "ksai_config.cpp" )
        configure_target(ksaivulkan)
endif()

#PrecompileStdHeaders(ksaivulkan)
someFlags(ksaivulkan)

