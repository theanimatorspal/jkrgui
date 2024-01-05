#pragma once
#include "VulkanDevice.hpp"

namespace ksai {
class VulkanSampler {
public:
    VulkanSampler(const VulkanDevice& inDevice)
        : mDevice(inDevice.GetDeviceHandle())
    {
        auto VulkanSamplerCreateInfo = vk::SamplerCreateInfo();
        VulkanSamplerCreateInfo.setMinFilter(vk::Filter::eLinear);
        mSampler = mDevice.createSampler(VulkanSamplerCreateInfo);
    }
    ~VulkanSampler()
    {
        mDevice.waitIdle();
        mDevice.destroySampler(mSampler);
    }
    GETTER& GetSamplerHandle() const { return mSampler; }

private:
    const vk::Device& mDevice;
    vk::Sampler mSampler;
};
}
