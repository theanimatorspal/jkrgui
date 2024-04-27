#include "VulkanDescriptorSet.hpp"

using namespace ksai;

ksai::VulkanDescriptorSet::VulkanDescriptorSet(
     const VulkanDevice& inDevice,
     const VulkanDescriptorPoolBase& inDescriptorPool,
     const VulkanDescriptorSetLayoutBase& inDescriptorSetLayout)
    : mDevice(inDevice.GetDeviceHandle()) {
    vk::DescriptorSetAllocateInfo DescriptorSetAllocateInfo(
         inDescriptorPool.GetDescriptorPoolHandle(),
         inDescriptorSetLayout.GetDescriptorLayoutHandle());
    mVulkanDescriptorSetHandles =
         inDevice.GetDeviceHandle().allocateDescriptorSets(DescriptorSetAllocateInfo);
}

ksai::VulkanDescriptorSet::VulkanDescriptorSet(
     const VulkanDevice& inDevice,
     const VulkanDescriptorPoolBase& inDescriptorPool,
     const VulkanDescriptorSetLayoutBase& inDescriptorSetLayout,
     uint32_t inNoOfVarDescriptorSets)
    : mDevice(inDevice.GetDeviceHandle()) {
    vk::DescriptorSetAllocateInfo DescriptorSetAllocateInfo(
         inDescriptorPool.GetDescriptorPoolHandle(),
         inDescriptorSetLayout.GetDescriptorLayoutHandle());
    std::array<const uint32_t, 1> DescriptorCounts = {inNoOfVarDescriptorSets};
    vk::DescriptorSetVariableDescriptorCountAllocateInfo VariableInfo(DescriptorCounts);

    vk::StructureChain<vk::DescriptorSetAllocateInfo,
                       vk::DescriptorSetVariableDescriptorCountAllocateInfo>
         CreateInfoC(DescriptorSetAllocateInfo, VariableInfo);

    mVulkanDescriptorSetHandles = inDevice.GetDeviceHandle().allocateDescriptorSets(
         CreateInfoC.get<vk::DescriptorSetAllocateInfo>());
}

void VulkanDescriptorSet::Bind(vk::PipelineBindPoint inBindPoint,
                               const VulkanCommandBuffer& inBuffer,
                               const VulkanPipelineLayoutBase& inPipelineLayout,
                               int inSet) {
    inBuffer.GetCommandBufferHandle().bindDescriptorSets(inBindPoint,
                                                         inPipelineLayout.GetPipelineLayoutHandle(),
                                                         inSet,
                                                         mVulkanDescriptorSetHandles[inSet],
                                                         {});
}