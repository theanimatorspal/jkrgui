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
          VulkanDescriptorSet(const VulkanDevice& inDevice,
                              const VulkanDescriptorPoolBase& inDescriptorPool,
                              const VulkanDescriptorSetLayoutBase& inDescriptorSetLayout);
          VulkanDescriptorSet(const VulkanDevice& inDevice,
                              const VulkanDescriptorPoolBase& inDescriptorPool,
                              const VulkanDescriptorSetLayoutBase& inDescriptorSetLayout,
                              uint32_t inNoOfVarDescriptorSets);
          void Bind(vk::PipelineBindPoint inBindPoint, const VulkanCommandBuffer& inBuffer, const VulkanPipelineLayoutBase& inBase);
          GETTER& GetDescriptorSetHandle() const { return mVulkanDescriptorSetHandle; }

      private:
          const vk::Device& mDevice;
          vk::DescriptorSet mVulkanDescriptorSetHandle;
};

inline void
VulkanDescriptorSet::Bind(vk::PipelineBindPoint inBindPoint, const VulkanCommandBuffer& inBuffer, const VulkanPipelineLayoutBase& inPipelineLayout) {
          inBuffer.GetCommandBufferHandle().bindDescriptorSets(
           inBindPoint, inPipelineLayout.GetPipelineLayoutHandle(), 0, mVulkanDescriptorSetHandle, {});
}

} // namespace ksai
