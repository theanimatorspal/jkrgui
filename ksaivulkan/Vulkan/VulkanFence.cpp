#include "VulkanFence.hpp"
using namespace ksai;
#ifdef max
#undef max
#endif
#include <iostream>
#include <limits>


VulkanFence::VulkanFence(const VulkanDevice& inDevice) : mDevice(inDevice.GetDeviceHandle()) {
    auto FenceCreateInfo = vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled);
    mFence               = mDevice.createFence(FenceCreateInfo);
}

VulkanFence::~VulkanFence() { mDevice.destroyFence(mFence); }

vk::Result VulkanFence::Wait() const {
    return mDevice.waitForFences(mFence, true, std::numeric_limits<uint32_t>::max());
}

void VulkanFence::Reset() const { mDevice.resetFences(mFence); }
