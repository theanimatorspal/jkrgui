#include "VulkanCommandPool.hpp"
using namespace ksai;

VulkanCommandPool::VulkanCommandPool(const VulkanDevice & inDevice, const VulkanQueueContext& inContext) : mDevice(inDevice.GetDeviceHandle())
{
	auto CommandPoolCreateInfo = vk::CommandPoolCreateInfo(
		vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		inContext.GetGraphicsQueueFamilyIndex()
	);
	mPool = mDevice.createCommandPool(CommandPoolCreateInfo);
}

VulkanCommandPool::~VulkanCommandPool()
{
	mDevice.waitIdle();
	mDevice.destroyCommandPool(mPool);
}
