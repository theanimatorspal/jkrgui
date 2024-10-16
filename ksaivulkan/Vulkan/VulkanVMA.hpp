#pragma once
#include "VulkanDevice.hpp"
#include <Vulkan/Config.hpp>
#include <vma/vk_mem_alloc.h>

namespace ksai {
class VulkanVMA {
    public:
    operator VmaAllocator() const { return mAllocator; }
    VulkanVMA(VulkanInstance &inInstance, VulkanDevice &inDevice) {
        auto info             = VmaAllocatorCreateInfo();
        info.device           = inDevice.GetDeviceHandle();
        info.instance         = inInstance.GetInstanceHandle();
        info.vulkanApiVersion = VK_API_VERSION_1_0;
        info.physicalDevice   = inDevice.GetPhysicalDeviceHandle();
        vmaCreateAllocator(&info, &mAllocator);
    }
    ~VulkanVMA() { vmaDestroyAllocator(mAllocator); }
    GETTER &GetVMAHandle() const { return mAllocator; }

    private:
    VmaAllocator mAllocator;
};
} // namespace ksai
