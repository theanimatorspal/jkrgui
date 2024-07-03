#include "VulkanPhysicalDevice.hpp"
#include <cassert>
using namespace ksai;

VulkanPhysicalDevice::VulkanPhysicalDevice(const VulkanInstance& inInstance)
    : mInstance(&inInstance.GetInstanceHandle()) {
    mPhysicalDevice = mInstance->enumeratePhysicalDevices().front();
    ksai_print("NO OF PHYSICAL DEVICES:");
    ksai_print(std::to_string(mInstance->enumeratePhysicalDevices().size()).c_str());
    mPhysicalDeviceProperties = mPhysicalDevice.getProperties();
    assert(mPhysicalDeviceProperties.limits.framebufferColorSampleCounts &
           vk::SampleCountFlagBits::e4);
}

ksai::VulkanPhysicalDevice::~VulkanPhysicalDevice() {}
