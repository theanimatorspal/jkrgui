#include "VulkanDescriptorSet.hpp"

using namespace ksai;
static std::mutex mMutex;

ksai::VulkanDescriptorSet::VulkanDescriptorSet(
     const VulkanDevice &inDevice,
     const VulkanDescriptorPoolBase &inDescriptorPool,
     const VulkanDescriptorSetLayoutBase &inDescriptorSetLayout) {
    Init({&inDevice, &inDescriptorPool, &inDescriptorSetLayout});
}

// TODO Fix this
ksai::VulkanDescriptorSet::VulkanDescriptorSet(
     const VulkanDevice &inDevice,
     const VulkanDescriptorPoolBase &inDescriptorPool,
     const VulkanDescriptorSetLayoutBase &inDescriptorSetLayout,
     uint32_t inNoOfVarDescriptorSets) {
    vk::DescriptorSetAllocateInfo DescriptorSetAllocateInfo(
         inDescriptorPool.GetDescriptorPoolHandle(),
         inDescriptorSetLayout.GetDescriptorLayoutHandle());
    std::array<const uint32_t, 1> DescriptorCounts = {inNoOfVarDescriptorSets};
    vk::DescriptorSetVariableDescriptorCountAllocateInfo VariableInfo(DescriptorCounts);

    vk::StructureChain<vk::DescriptorSetAllocateInfo,
                       vk::DescriptorSetVariableDescriptorCountAllocateInfo>
         CreateInfoC(DescriptorSetAllocateInfo, VariableInfo);

    {
        std::scoped_lock<std::mutex> lock(mMutex);
        mVulkanDescriptorSetHandles = inDevice.GetDeviceHandle().allocateDescriptorSets(
             CreateInfoC.get<vk::DescriptorSetAllocateInfo>());
    }
}

void VulkanDescriptorSet::Bind(vk::PipelineBindPoint inBindPoint,
                               const VulkanCommandBuffer &inBuffer,
                               const VulkanPipelineLayoutBase &inPipelineLayout,
                               int inSet) {
    inBuffer.GetCommandBufferHandle().bindDescriptorSets(inBindPoint,
                                                         inPipelineLayout.GetPipelineLayoutHandle(),
                                                         inSet,
                                                         mVulkanDescriptorSetHandles[inSet],
                                                         {});
}

void VulkanDescriptorSet::Init(CreateInfo info) {
    mDevice         = &info.inDevice->GetDeviceHandle();
    mDescriptorPool = info.inDescriptorPool;
    vk::DescriptorSetAllocateInfo DescriptorSetAllocateInfo(
         info.inDescriptorPool->GetDescriptorPoolHandle(),
         info.inDescriptorSetLayout->GetDescriptorLayoutHandle());
    {
        std::scoped_lock<std::mutex> lock(mMutex);
        mVulkanDescriptorSetHandles =
             info.inDevice->GetDeviceHandle().allocateDescriptorSets(DescriptorSetAllocateInfo);
    }
    mInitialized = true;
}
