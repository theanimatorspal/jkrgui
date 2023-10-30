#include "VulkanCommandBuffer.hpp"

using namespace ksai;

VulkanCommandBuffer::VulkanCommandBuffer(const VulkanDevice& inDevice, const VulkanCommandPool& inPool) : mDevice(inDevice.GetDeviceHandle())
{
	auto CommandBufferAllocateInfo = vk::CommandBufferAllocateInfo(inPool.GetCommandPoolHandle(), vk::CommandBufferLevel::ePrimary, 1);
	mBuffer = mDevice.allocateCommandBuffers(CommandBufferAllocateInfo).front();
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
}

void VulkanCommandBuffer::EndRenderPass() const
{
	mBuffer.endRenderPass();
}

