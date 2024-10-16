#pragma once
#include "VulkanDevice.hpp"
#include "VulkanImageContext.hpp"

namespace ksai {
class VulkanSampler {
    public:
    struct CreateInfo {
        VulkanDevice *mDevice                  = nullptr;
        ImageContext mImageContext             = ImageContext::Default;
        float mMinLod                          = 0.0f;
        float mMaxLod                          = 1.0f;
        opt<vk::SamplerCreateInfo> mCreateInfo = std::nullopt;
    };

    VulkanSampler() = default;
    ~VulkanSampler();
    VulkanSampler(VulkanSampler &other)             = delete;
    VulkanSampler &operator=(VulkanSampler &other)  = delete;
    VulkanSampler(VulkanSampler &&other)            = default;
    VulkanSampler &operator=(VulkanSampler &&other) = default;
    operator vk::Sampler() const { return mSampler; }

    void Init(CreateInfo inCreateInfo);
    void Destroy();
    // TODO Make this better
    VulkanSampler(VulkanDevice &inDevice,
                  ImageContext inImageContext = ImageContext::Default,
                  float inMinLod              = 0.0f,
                  float inMaxLoad             = 1.0f);
    VulkanSampler(VulkanDevice &inDevice, vk::SamplerCreateInfo inCreateInfo);
    GETTER &GetSamplerHandle() const { return mSampler; }

    private:
    vk::Device *mDevice;
    vk::Sampler mSampler;
    bool mInitialized = false;
};
} // namespace ksai
