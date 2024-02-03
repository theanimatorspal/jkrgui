include("../CMakeCommon.cmake")

#if apple then no tracy
if(APPLE OR ANDROID)
        file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS
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
        "ksai_config.hpp"
        "ksai_image.hpp"
        "ksai_thread.hpp"
        )
	   if(ANDROID)
		   add_library(ksaivulkan ${app_SRC})
		   configure_target(ksaivulkan)
	   else()
		   add_library(ksaivulkan ${app_SRC})
	   endif()
else()
        file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS
        "*.hpp"
        "*.cpp"
        "*.h"
        "*.c"
        )
        add_library(ksaivulkan ${app_SRC} "ksai_image.hpp" "ksai_thread.hpp" "Vendor/NanoBench/nanobench.h" "Vendor/NanoBench/nanobench.cpp" "ksai_config.cpp")
endif()

someFlags(ksaivulkan)

