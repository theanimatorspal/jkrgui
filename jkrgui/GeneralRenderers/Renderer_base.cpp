#include "Renderer_base.hpp"

void Jkr::Renderer::Renderer_base::CreateStagingBuffers(const Instance& inInstance, size_t inVertexStagingBufferSizeInBytes, size_t inIndexStagingBufferSizeInBytes)
{
	mStagingVertexBuffer = MakeUp<StagingBuffer>(inInstance.GetVMA(), inInstance.GetDevice(), inVertexStagingBufferSizeInBytes);
	mStagingVertexBuffer->MapMemoryRegion(&mVertexStagingBufferMemoryMapPtr);
	mStagingIndexBuffer = MakeUp<StagingBuffer>(inInstance.GetVMA(), inInstance.GetDevice(), inIndexStagingBufferSizeInBytes);
	mStagingIndexBuffer->MapMemoryRegion(&mIndexStagingBufferMemoryMapPtr);
	std::memset(mVertexStagingBufferMemoryMapPtr, 0, inVertexStagingBufferSizeInBytes);
	std::memset(mIndexStagingBufferMemoryMapPtr, 0, inIndexStagingBufferSizeInBytes);
	mStagingVertexBufferSize = inVertexStagingBufferSizeInBytes;
	mStagingIndexBufferSize = inIndexStagingBufferSizeInBytes;
}

void Jkr::Renderer::Renderer_base::CopyToStagingBuffers(void* inVertexData, void* inIndexData, size_t inVertexOffset, size_t inIndexOffset, size_t inVertexSize, size_t inIndexSize)
{
	std::memcpy(
		(char*)mVertexStagingBufferMemoryMapPtr + inVertexOffset,
		(char*)inVertexData + inVertexOffset,
		inVertexSize
	);
	mStagingVertexBuffer->FlushMemoryRanges(inVertexOffset, inVertexSize);

	std::memcpy(
		(char*)mIndexStagingBufferMemoryMapPtr + inIndexOffset,
		(char*)inIndexData + inIndexOffset,
		inIndexSize
	);
	mStagingIndexBuffer->FlushMemoryRanges(inIndexOffset, inIndexSize);
}

void Jkr::Renderer::Renderer_base::ResizeStagingBuffer(const Instance& inInstance, size_t inVertexStagingBufferSizeInBytes, size_t inIndexStagingBufferSizeInBytes)
{
	mStagingVertexBuffer.reset();
	mStagingIndexBuffer.reset();
	CreateStagingBuffers(inInstance, inVertexStagingBufferSizeInBytes, inIndexStagingBufferSizeInBytes);
}

void Jkr::Renderer::Renderer_base::CmdCopyToPrimitiveFromStagingBufferSingleTime(const Instance& inInstance, GUIPrimitive& inPrimitive, GUIWindow& inWindow, size_t inVertexMemorySizeToBeBarriered, size_t inIndexMemorySizeToBeBarriered)
{
	inInstance.GetCommandBuffers()[inWindow.GetCurrentFrame()].GetCommandBufferHandle().copyBuffer(
		mStagingVertexBuffer->GetBufferHandle(),
		inPrimitive.GetVertexBufferPtr()->GetBufferHandle(),
		mVertexCopyRegionsToBeSubmittedOnce
	);

	inPrimitive.GetVertexBufferPtr()->SetBarrier(
		inInstance.GetCommandBuffers()[inWindow.GetCurrentFrame()],
		0,
		inVertexMemorySizeToBeBarriered,
		vk::AccessFlagBits::eTransferWrite,
		vk::AccessFlagBits::eVertexAttributeRead,
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eVertexInput
	);

	inInstance.GetCommandBuffers()[inWindow.GetCurrentFrame()].GetCommandBufferHandle().copyBuffer(
		mStagingIndexBuffer->GetBufferHandle(),
		inPrimitive.GetIndexBufferPtr()->GetBufferHandle(),
		mIndexCopyRegionsToBeSubmittedOnce
	);

	inPrimitive.GetIndexBufferPtr()->SetBarrier(
		inInstance.GetCommandBuffers()[inWindow.GetCurrentFrame()],
		0,
		inIndexMemorySizeToBeBarriered,
		vk::AccessFlagBits::eTransferWrite,
		vk::AccessFlagBits::eVertexAttributeRead,
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eVertexInput
	);
	mVertexCopyRegionsToBeSubmittedOnce.clear();
	mIndexCopyRegionsToBeSubmittedOnce.clear();
}

void Jkr::Renderer::Renderer_base::CmdCopyToPrimitiveFromStagingBufferEachFrame(const Instance& inInstance, GUIPrimitive& inPrimitive, GUIWindow& inWindow, size_t inVertexMemorySizeToBeBarriered, size_t inIndexMemorySizeToBeBarriered)
{
	inInstance.GetCommandBuffers()[inWindow.GetCurrentFrame()].GetCommandBufferHandle().copyBuffer(
		mStagingVertexBuffer->GetBufferHandle(),
		inPrimitive.GetVertexBufferPtr()->GetBufferHandle(),
		mVertexCopyRegionsToBeSubmittedEachFrame
	);

	inPrimitive.GetVertexBufferPtr()->SetBarrier(
		inInstance.GetCommandBuffers()[inWindow.GetCurrentFrame()],
		0,
		inVertexMemorySizeToBeBarriered,
		vk::AccessFlagBits::eTransferWrite,
		vk::AccessFlagBits::eVertexAttributeRead,
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eVertexInput
	);

	inInstance.GetCommandBuffers()[inWindow.GetCurrentFrame()].GetCommandBufferHandle().copyBuffer(
		mStagingIndexBuffer->GetBufferHandle(),
		inPrimitive.GetIndexBufferPtr()->GetBufferHandle(),
		mIndexCopyRegionsToBeSubmittedEachFrame
	);

	inPrimitive.GetIndexBufferPtr()->SetBarrier(
		inInstance.GetCommandBuffers()[inWindow.GetCurrentFrame()],
		0,
		inIndexMemorySizeToBeBarriered,
		vk::AccessFlagBits::eTransferWrite,
		vk::AccessFlagBits::eVertexAttributeRead,
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eVertexInput
	);
}

void Jkr::Renderer::Renderer_base::RegisterBufferCopyRegionToPrimitiveFromStagingFirstFrameOnly(size_t inVertexOffset, size_t inIndexOffset, size_t inVertexSize, size_t inIndexSize)
{
	mVertexCopyRegionsToBeSubmittedOnce.push_back(vk::BufferCopy(inVertexOffset, inVertexOffset, inVertexSize));
	mIndexCopyRegionsToBeSubmittedOnce.push_back(vk::BufferCopy(inIndexOffset, inIndexOffset, inIndexSize));
}

void Jkr::Renderer::Renderer_base::RegisterBufferCopyRegionToPrimitiveFromStagingEachFrame(size_t inVertexOffset, size_t inIndexOffset, size_t inVertexSize, size_t inIndexSize)
{
	mVertexCopyRegionsToBeSubmittedEachFrame.push_back(vk::BufferCopy(inVertexOffset, inVertexOffset, inVertexSize));
	mIndexCopyRegionsToBeSubmittedEachFrame.push_back(vk::BufferCopy(inIndexOffset, inIndexOffset, inVertexSize));
}
