#include "Renderer_base.hpp"

#ifndef JKR_NO_STAGING_BUFFERS
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

void Jkr::Renderer::Renderer_base::CmdCopyToPrimitiveFromStagingBuffer(const Instance& inInstance, Primitive& inPrimitive, Window& inWindow, size_t inVertexMemorySizeToBeBarriered, size_t inIndexMemorySizeToBeBarriered)
{
	inInstance.GetCommandBuffers()[inWindow.GetCurrentFrame()].GetCommandBufferHandle().copyBuffer(
		mStagingVertexBuffer->GetBufferHandle(),
		inPrimitive.GetVertexBufferPtr()->GetBufferHandle(),
		mVertexCopyRegionsToBeSubmitted
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
		mIndexCopyRegionsToBeSubmitted
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
	mVertexCopyRegionsToBeSubmitted.clear();
	mIndexCopyRegionsToBeSubmitted.clear();
}


void Jkr::Renderer::Renderer_base::RegisterBufferCopyRegionToPrimitiveFromStaging(size_t inVertexOffset, size_t inIndexOffset, size_t inVertexSize, size_t inIndexSize)
{
	mVertexCopyRegionsToBeSubmitted.push_back(vk::BufferCopy(inVertexOffset, inVertexOffset, inVertexSize));
	mIndexCopyRegionsToBeSubmitted.push_back(vk::BufferCopy(inIndexOffset, inIndexOffset, inIndexSize));
}

#else
void Jkr::Renderer::Renderer_base::CopyToPrimitive(Primitive& inPrimitive, void* inVertexData, void* inIndexData, size_t inVertexOffset, size_t inIndexOffset, size_t inVertexSize, size_t inIndexSize)
{
	std::memcpy(
		(char*)inPrimitive.GetVertexMemoryMapPtr() + inVertexOffset,
		(char*)inVertexData + inVertexOffset,
		inVertexSize
	);
	inPrimitive.GetVertexBufferPtr()->FlushMemoryRanges(inVertexOffset, inVertexSize);

	std::memcpy(
		(char*)inPrimitive.GetIndexMemoryMapPtr() + inIndexOffset,
		(char*)inIndexData + inIndexOffset,
		inIndexSize
	);
	inPrimitive.GetIndexBufferPtr()->FlushMemoryRanges(inIndexOffset, inIndexSize);
}
#endif
