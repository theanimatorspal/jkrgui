#pragma once
#include <Vulkan/Pipeline/VulkanDescriptorSet.hpp>
#include <Vulkan/VulkanDevice.hpp>
#include <Vulkan/VulkanSampler.hpp>
#include <VulkanBuffer.hpp>

namespace ksai {
class VulkanDescriptorUpdateHandler {
    public:
    VulkanDescriptorUpdateHandler(VulkanDevice &inDevice) : mDevice(inDevice) {}
    void RW(BufferContext inBufferContext,
            VulkanDescriptorSet &inDescriptorSet,
            VulkanBufferBase &inBuffer,
            vk::DeviceSize inOffset,
            uint32_t inDstBinding,
            uint32_t inDstArrayElement,
            uint32_t inDstSet = 0);

    void RW(ImageContext inImageContext,
            VulkanDescriptorSet &inDescriptorSet,
            VulkanImageBase &inImage,
            VulkanSampler &inSampler,
            uint32_t inDstBinding,
            uint32_t inDstArrayElement,
            uint32_t inDstSet = 0);

    void RW(VulkanDescriptorSet &inDescriptorSet,
            VulkanSampler &inSampler,
            uint32_t inDstBinding,
            uint32_t inDstArrayElement,
            uint32_t inDstSet = 0);

    private:
    VulkanDevice &mDevice;
    std::vector<vk::WriteDescriptorSet> mDescriptorWrites;
};
} // namespace ksai