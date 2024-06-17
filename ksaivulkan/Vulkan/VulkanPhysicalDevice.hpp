#pragma once

#include "VulkanInstance.hpp"

namespace ksai {
enum class MemoryType { DeviceLocal, HostVisibleAndCoherenet, HostVisibleAndHostCached };
}

namespace ksai {

class VulkanPhysicalDevice {
    public:
    operator vk::PhysicalDevice() const { return mPhysicalDevice; }
    VulkanPhysicalDevice(const VulkanInstance& inInstance);
    ~VulkanPhysicalDevice();
    GETTER& GetPhysicalDeviceHandle() const { return mPhysicalDevice; }

    private:
    const vk::Instance& mInstance;
    vk::PhysicalDevice mPhysicalDevice;
    vk::PhysicalDeviceProperties mPhysicalDeviceProperties;
    vk::SampleCountFlags mMaxSampleCount;
};
} // namespace ksai
