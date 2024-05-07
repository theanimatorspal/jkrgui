#include "VulkanSampler.hpp"

using namespace ksai;
VulkanSampler::VulkanSampler(const VulkanDevice& inDevice, ImageContext inImageContext)
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

VulkanSampler::~VulkanSampler() { mDevice.destroySampler(mSampler); }