#include "VulkanPhysicalDevice.hpp"
#include <cassert>
using namespace ksai;

VulkanPhysicalDevice::VulkanPhysicalDevice(VulkanInstance &inInstance)
    : mInstance(&inInstance.GetInstanceHandle()) {
    mVulkanInstance = &inInstance;
    mPhysicalDevice = mInstance->enumeratePhysicalDevices().front();
    Log("No of Physical Devices: " + std::to_string(mInstance->enumeratePhysicalDevices().size()));
    mPhysicalDeviceProperties = mPhysicalDevice.getProperties();
    assert(mPhysicalDeviceProperties.limits.framebufferColorSampleCounts &
           vk::SampleCountFlagBits::e4);
}

ksai::VulkanPhysicalDevice::~VulkanPhysicalDevice() {}
