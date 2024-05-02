#pragma once
#include "VulkanDevice.hpp"
#include "VulkanImageContext.hpp"

namespace ksai {
class VulkanSampler {
    public:
    // TODO Make this better
    VulkanSampler(const VulkanDevice& inDevice,
                  ImageContext inImageContext = ImageContext::Default);
    ~VulkanSampler();
    GETTER& GetSamplerHandle() const { return mSampler; }

    private:
    const vk::Device& mDevice;
    vk::Sampler mSampler;
};
} // namespace ksai
