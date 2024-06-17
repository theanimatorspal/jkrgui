#pragma once
#include "VulkanDevice.hpp"
#include "VulkanImageContext.hpp"

namespace ksai {
class VulkanSampler {
    public:
    operator vk::Sampler() const { return mSampler; }
    // TODO Make this better
    VulkanSampler(const VulkanDevice& inDevice,
                  ImageContext inImageContext = ImageContext::Default,
                  float inMinLod              = 0.0f,
                  float inMaxLoad             = 1.0f);
    ~VulkanSampler();
    GETTER& GetSamplerHandle() const { return mSampler; }

    private:
    const vk::Device& mDevice;
    vk::Sampler mSampler;
};
} // namespace ksai
