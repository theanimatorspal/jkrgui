#pragma once
#include "Pipeline/VulkanPipelineLayout.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanDescriptorPool.hpp"
#include "VulkanDescriptorSetLayout.hpp"
#include "vulkan/vulkan_enums.hpp"
#include <Vulkan/VulkanBuffer.hpp>
#include <Vulkan/VulkanDevice.hpp>
#include <vector>

namespace ksai {
class VulkanDescriptorSet {
    public:
    void Bind(vk::PipelineBindPoint inBindPoint,
              const VulkanCommandBuffer& inBuffer,
              const VulkanPipelineLayoutBase& inBase,
              int inSet = 0);
    GETTER& GetDescriptorSetHandle() const { return mVulkanDescriptorSetHandles; }

    VulkanDescriptorSet(const VulkanDevice& inDevice,
                        const VulkanDescriptorPoolBase& inDescriptorPool,
                        const VulkanDescriptorSetLayoutBase& inDescriptorSetLayout);
    VulkanDescriptorSet(const VulkanDevice& inDevice,
                        const VulkanDescriptorPoolBase& inDescriptorPool,
                        const VulkanDescriptorSetLayoutBase& inDescriptorSetLayout,
                        uint32_t inNoOfVarDescriptorSets);

    private:
    const vk::Device& mDevice;
    const VulkanDescriptorPoolBase& mDescriptorPool;
    std::vector<vk::DescriptorSet> mVulkanDescriptorSetHandles;
};

} // namespace ksai
