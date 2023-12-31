#pragma once
#include "VulkanQueueContext.hpp"
#include "VulkanDevice.hpp"

namespace ksai {

	class VulkanCommandPool
	{
	public:
		VulkanCommandPool(const VulkanDevice& inDevice, const VulkanQueueContext& inContext);
		~VulkanCommandPool();
		GETTER& GetCommandPoolHandle() const { return mPool; }
	private:
		const vk::Device& mDevice;
		vk::CommandPool mPool;
	};

}
