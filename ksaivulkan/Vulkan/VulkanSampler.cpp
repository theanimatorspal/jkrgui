#include "VulkanSampler.hpp"

using namespace ksai;
VulkanSampler::VulkanSampler(const VulkanDevice &inDevice,
                             ImageContext inImageContext,
                             float inMinLod,
                             float inMaxLod) {
    Init({&inDevice, inImageContext, inMinLod, inMaxLod});
}

VulkanSampler::VulkanSampler(const VulkanDevice &inDevice, vk::SamplerCreateInfo inCreateInfo) {
    Init({.mDevice = &inDevice, .mCreateInfo = inCreateInfo});
}

VulkanSampler::~VulkanSampler() {
    if (mInitialized) {
        Destroy();
    }
}

void VulkanSampler::Init(CreateInfo inCreateInfo) {
    mDevice                                       = &inCreateInfo.mDevice->GetDeviceHandle();
    vk::SamplerCreateInfo VulkanSamplerCreateInfo = vk::SamplerCreateInfo();
    VulkanSamplerCreateInfo.setMinFilter(vk::Filter::eLinear);
    if (inCreateInfo.mImageContext == ImageContext::DepthImage) {
        VulkanSamplerCreateInfo.setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
             .setAddressModeW(vk::SamplerAddressMode::eClampToEdge)
             .setAddressModeV(vk::SamplerAddressMode::eClampToEdge);
    }
    VulkanSamplerCreateInfo.setMinLod(inCreateInfo.mMinLod).setMaxLod(inCreateInfo.mMaxLod);
    if (inCreateInfo.mCreateInfo.has_value())
        VulkanSamplerCreateInfo = inCreateInfo.mCreateInfo.value();
    mSampler     = mDevice->createSampler(VulkanSamplerCreateInfo);
    mInitialized = true;
}
void VulkanSampler::Destroy() {
    mDevice->waitIdle();
    mDevice->destroySampler(mSampler);
}