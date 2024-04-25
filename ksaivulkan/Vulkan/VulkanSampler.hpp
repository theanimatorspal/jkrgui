#pragma once
#include "VulkanDevice.hpp"
#include "VulkanImageContext.hpp"

namespace ksai {
class VulkanSampler {
    public:
    VulkanSampler(const VulkanDevice& inDevice, ImageContext inImageContext = ImageContext::Default)
        : mDevice(inDevice.GetDeviceHandle()) {
        vk::SamplerCreateInfo VulkanSamplerCreateInfo = vk::SamplerCreateInfo();
        VulkanSamplerCreateInfo.setMinFilter(vk::Filter::eLinear);
        if (inImageContext == ImageContext::DepthImage) {
            VulkanSamplerCreateInfo.setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
                 .setAddressModeW(vk::SamplerAddressMode::eClampToEdge)
                 .setAddressModeV(vk::SamplerAddressMode::eClampToEdge);
        }
        mSampler = mDevice.createSampler(VulkanSamplerCreateInfo);
    }
    ~VulkanSampler() {
        mDevice.waitIdle();
        mDevice.destroySampler(mSampler);
    }
    GETTER& GetSamplerHandle() const { return mSampler; }

    private:
    const vk::Device& mDevice;
    vk::Sampler mSampler;
};
} // namespace ksai
