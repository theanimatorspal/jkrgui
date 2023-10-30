#include "VulkanDescriptorSet.hpp"

ksai::VulkanDescriptorSet::VulkanDescriptorSet(const VulkanDevice& inDevice, const VulkanDescriptorPoolBase& inDescriptorPool, const VulkanDescriptorSetLayoutBase& inDescriptorSetLayout)
	: mDevice(inDevice.GetDeviceHandle())
{
    vk::DescriptorSetAllocateInfo DescriptorSetAllocateInfo(inDescriptorPool.GetDescriptorPoolHandle(), inDescriptorSetLayout.GetDescriptorLayoutHandle());
    mComputeDescriptorSet = inDevice.GetDeviceHandle().allocateDescriptorSets(DescriptorSetAllocateInfo).front();
}

ksai::VulkanDescriptorSet::VulkanDescriptorSet(const VulkanDevice& inDevice, const VulkanDescriptorPoolBase& inDescriptorPool, const VulkanDescriptorSetLayoutBase& inDescriptorSetLayout, uint32_t inNoOfVarDescriptorSets)
	: mDevice(inDevice.GetDeviceHandle())
{
    vk::DescriptorSetAllocateInfo DescriptorSetAllocateInfo(inDescriptorPool.GetDescriptorPoolHandle(), inDescriptorSetLayout.GetDescriptorLayoutHandle());
	 std::array<const uint32_t, 1> DescriptorCounts = { inNoOfVarDescriptorSets };
	 vk::DescriptorSetVariableDescriptorCountAllocateInfo VariableInfo(DescriptorCounts);

	 vk::StructureChain<vk::DescriptorSetAllocateInfo, vk::DescriptorSetVariableDescriptorCountAllocateInfo> CreateInfoC(DescriptorSetAllocateInfo, VariableInfo);

    mComputeDescriptorSet = inDevice.GetDeviceHandle().allocateDescriptorSets(CreateInfoC.get<vk::DescriptorSetAllocateInfo>()).front();
}
