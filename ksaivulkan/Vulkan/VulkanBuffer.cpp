#include "VulkanBuffer.hpp"

ksai::VulkanBufferBase::VulkanBufferBase(const VulkanDevice& inDevice)
	: mDevice(inDevice.GetDeviceHandle()),
	mPhysicalDevice(inDevice.GetPhysicalDeviceHandle())
{
}

void ksai::VulkanBufferBase::SubmitImmediateCmdCopyFrom(const VulkanQueue<QueueContext::Graphics>& inQueue, const VulkanCommandBuffer& inCmdBuffer, void* inData)
{
	uint32_t MemoryIndex;
	vk::DeviceSize MemorySize;
	GetMemoryTypeIndex(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, mBufferHandle, MemorySize, MemoryIndex);
	auto StagingBufferCreateInfo = vk::BufferCreateInfo(vk::BufferCreateFlags(), MemorySize, vk::BufferUsageFlagBits::eTransferSrc);
	auto StagingBuffer = mDevice.createBuffer(StagingBufferCreateInfo);
	auto StagingBufferMemory = mDevice.allocateMemory(vk::MemoryAllocateInfo(MemorySize, MemoryIndex));
	mDevice.bindBufferMemory(StagingBuffer, StagingBufferMemory, 0);
	uint8_t* pData = static_cast<uint8_t*>(mDevice.mapMemory(StagingBufferMemory, 0, MemorySize));
	std::memcpy(pData, inData, MemorySize);
	mDevice.unmapMemory(StagingBufferMemory);
	const vk::CommandBuffer& Cmd = inCmdBuffer.GetCommandBufferHandle();
	Cmd.begin(vk::CommandBufferBeginInfo());
	std::array<vk::BufferCopy, 1> Regions = { vk::BufferCopy(0, 0, MemorySize) };
	inCmdBuffer.GetCommandBufferHandle().copyBuffer(StagingBuffer, mBufferHandle, Regions);
	Cmd.end();
	inQueue.Submit<SubmitContext::SingleTime>(inCmdBuffer);
	mDevice.freeMemory(StagingBufferMemory);
	mDevice.destroyBuffer(StagingBuffer);
}

void ksai::VulkanBufferBase::CmdCopyFrom(const VulkanCommandBuffer& inCmdBuffer, VulkanBufferBase& inBuffer, vk::DeviceSize FromBufferOffset, vk::DeviceSize ToBufferOffset, vk::DeviceSize inSizeToCopy)
{
	vk::BufferCopy BufferCopyRegion = vk::BufferCopy(FromBufferOffset, ToBufferOffset, inSizeToCopy);
	inCmdBuffer.GetCommandBufferHandle().copyBuffer(inBuffer.mBufferHandle, mBufferHandle, BufferCopyRegion);

}

void ksai::VulkanBufferBase::SetBarrier(const VulkanCommandBuffer& inCmdBuffer, vk::DeviceSize inDstBufferOffset, vk::DeviceSize inSizeToCopy, vk::AccessFlags inBeforeAccess, vk::AccessFlags inAfterAccess, vk::PipelineStageFlags inBeforeStages, vk::PipelineStageFlags inAfterStages)
{
	vk::BufferMemoryBarrier Barrier(
		inBeforeAccess,
		inAfterAccess,
		VK_QUEUE_FAMILY_IGNORED,
		VK_QUEUE_FAMILY_IGNORED,
		mBufferHandle,
		inDstBufferOffset,
		inSizeToCopy
	);
	inCmdBuffer.GetCommandBufferHandle().pipelineBarrier(
		inBeforeStages,
		inAfterStages,
		vk::DependencyFlagBits::eByRegion,
		{},
		Barrier,
		{}
	);

}

void ksai::VulkanBufferBase::GetMemoryTypeIndex(vk::MemoryPropertyFlags inFlag, vk::Buffer inBuffer, vk::DeviceSize& outSize, uint32_t& outIndex)
{
	vk::PhysicalDeviceMemoryProperties memoryProperties = mPhysicalDevice.getMemoryProperties();
	vk::MemoryRequirements memoryRequirements = mDevice.getBufferMemoryRequirements(inBuffer);
	uint32_t typeBits = memoryRequirements.memoryTypeBits;
	uint32_t typeIndex = uint32_t(~0);
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if ((typeBits & 1) && ((memoryProperties.memoryTypes[i].propertyFlags & inFlag) == inFlag))
		{
			typeIndex = i;
			outIndex = typeIndex;
			outSize = memoryRequirements.size;
			return;
		}
		typeBits >>= 1;
	}
}


void ksai::VulkanBufferBase::CmdCopyFromImage(const VulkanCommandBuffer& inCmdBuffer, VulkanImageBase& inImage, vk::PipelineStageFlags inAfterStage, vk::AccessFlags inAfterAccessFlags)
{
	// TODO This has to be tested
	auto& Cmd = inCmdBuffer.GetCommandBufferHandle();
	auto& srcImageHandle = inImage.GetImageHandle();
	auto srcImageProp = inImage.GetImageProperty();
	auto& srcVulkanImage = inImage;
	auto srcImageLayout = inImage.GetImageProperty().mInitialImageLayout;
	vk::ImageSubresourceLayers SrcSubResource(srcImageProp.mImageAspect, 0, 0, srcImageProp.mArrayLayers);
	vk::BufferImageCopy CopyRegions(0, 0, 0, SrcSubResource, vk::Offset3D{}, vk::Extent3D{});


	srcVulkanImage.CmdTransitionImageLayout(inCmdBuffer,
		srcImageLayout,
		vk::ImageLayout::eTransferSrcOptimal,
		inAfterStage,
		vk::PipelineStageFlagBits::eTransfer,
		inAfterAccessFlags,
		vk::AccessFlagBits::eMemoryRead);

	Cmd.copyImageToBuffer(srcImageHandle, inImage.GetInitialImageLayout(), this->mBufferHandle, CopyRegions);

	srcVulkanImage.CmdTransitionImageLayout(inCmdBuffer,
		vk::ImageLayout::eTransferSrcOptimal,
		srcImageLayout,
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eFragmentShader,
		vk::AccessFlagBits::eMemoryRead,
		vk::AccessFlagBits::eMemoryRead
	);
}
