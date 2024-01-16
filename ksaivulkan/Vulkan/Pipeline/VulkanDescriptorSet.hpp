#pragma once
#include <vector>
#include <Vulkan/VulkanDevice.hpp>
#include "VulkanDescriptorPool.hpp"
#include "VulkanDescriptorSetLayout.hpp"
#include <Vulkan/VulkanBuffer.hpp>

namespace ksai {
	class VulkanDescriptorSet
	{
	public:
		VulkanDescriptorSet(const VulkanDevice& inDevice, const VulkanDescriptorPoolBase& inDescriptorPool, const VulkanDescriptorSetLayoutBase& inDescriptorSetLayout);
		VulkanDescriptorSet(const VulkanDevice& inDevice, const VulkanDescriptorPoolBase& inDescriptorPool, const VulkanDescriptorSetLayoutBase& inDescriptorSetLayout, uint32_t inNoOfVarDescriptorSets);
		GETTER& GetDescriptorSetHandle() const { return mComputeDescriptorSet; }
	private:
		const vk::Device& mDevice;
		vk::DescriptorSet mComputeDescriptorSet;
	};
}

namespace ksai {
}