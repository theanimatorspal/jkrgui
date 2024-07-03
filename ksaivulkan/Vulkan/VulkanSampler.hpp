#pragma once
#include "VulkanDevice.hpp"
#include "VulkanImageContext.hpp"

namespace ksai {
class VulkanSampler {
    public:
    struct CreateInfo {
        const VulkanDevice *mDevice            = nullptr;
        ImageContext mImageContext             = ImageContext::Default;
        float mMinLod                          = 0.0f;
        float mMaxLod                          = 1.0f;
        opt<vk::SamplerCreateInfo> mCreateInfo = std::nullopt;
    };

    VulkanSampler() = default;
    ~VulkanSampler();
    VulkanSampler(const VulkanSampler &other)            = delete;
    VulkanSampler &operator=(const VulkanSampler &other) = delete;
    VulkanSampler(VulkanSampler &&other)                 = default;
    VulkanSampler &operator=(VulkanSampler &&other)      = default;
    operator vk::Sampler() const { return mSampler; }

    void Init(CreateInfo inCreateInfo);
    void Destroy();
    // TODO Make this better
    VulkanSampler(const VulkanDevice &inDevice,
                  ImageContext inImageContext = ImageContext::Default,
                  float inMinLod              = 0.0f,
                  float inMaxLoad             = 1.0f);
    VulkanSampler(const VulkanDevice &inDevice, vk::SamplerCreateInfo inCreateInfo);
    GETTER &GetSamplerHandle() const { return mSampler; }

    private:
    const vk::Device *mDevice;
    vk::Sampler mSampler;
    bool mInitialized = false;
};
} // namespace ksai
