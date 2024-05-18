# Set the path to Vulkan SDK based on the platform
set(CMAKE_JKRGUI_DIR "C:/Users/sansk/OneDrive/Pictures/jkrgui")
if(APPLE)
    set(VULKAN_SDK "/Users/bishaljaiswal/VulkanSDK")
    set(VULKAN_VERSION "1.3.268.1")
elseif(WIN32)
    set(VULKAN_SDK "C:/VulkanSDK")
    set(VULKAN_VERSION "1.3.275.0")
endif()

# For Now