#pragma once
#include <Vulkan/Pipeline/VulkanDescriptorSet.hpp>
#include <Vulkan/VulkanDevice.hpp>
#include <Vulkan/VulkanSampler.hpp>
#include <VulkanBuffer.hpp>

namespace ksai {
class VulkanDescriptorUpdateHandler {
    public:
    VulkanDescriptorUpdateHandler(const VulkanDevice& inDevice) : mDevice(inDevice) {}
    void RW(BufferContext inBufferContext,
            const VulkanDescriptorSet& inDescriptorSet,
            const VulkanBufferBase& inBuffer,
            vk::DeviceSize inOffset,
            uint32_t inDstBinding,
            uint32_t inDstArrayElement,
            uint32_t inDstSet = 0);

    void RW(ImageContext inImageContext,
            const VulkanDescriptorSet& inDescriptorSet,
            const VulkanImageBase& inImage,
            const VulkanSampler& inSampler,
            uint32_t inDstBinding,
            uint32_t inDstArrayElement,
            uint32_t inDstSet = 0);

    void RW(const VulkanDescriptorSet& inDescriptorSet,
            const VulkanSampler& inSampler,
            uint32_t inDstBinding,
            uint32_t inDstArrayElement,
            uint32_t inDstSet = 0);

    private:
    const VulkanDevice& mDevice;
    std::vector<vk::WriteDescriptorSet> mDescriptorWrites;
};
} // namespace ksai