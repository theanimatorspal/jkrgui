# Set the path to Vulkan SDK based on the platform
set(CMAKE_JKRGUI_DIR "C:/Users/sansk/OneDrive/Pictures/jkrgui")
if(APPLE)
    set(CMAKE_JKRGUI_DIR "/Users/bishaljaiswal/Documents/Minor Project 2/jkrgui")
    set(VULKAN_SDK "/Users/bishaljaiswal/VulkanSDK")
    set(VULKAN_VERSION "1.4.304.0")
elseif(WIN32)
    set(VULKAN_SDK "C:/VulkanSDK")
    set(VULKAN_VERSION "1.4.304.0")
endif()

# For Now