#pragma once
#include "GUI2DRenderer.hpp"

namespace Jkr::Renderer {
	class Renderer_base
	{
	public:
		using ImageType = Jkr::GUIPainterParameter<Jkr::GUIPainterParameterContext::StorageImage>;
		using StagingBuffer = Jkr::VulkanBufferVMA<BufferContext::Staging, MemoryType::HostVisibleAndHostCached>;
		using SamplerType = Jkr::GUIPainterParameter<Jkr::GUIPainterParameterContext::UniformSampler>;
		using UniformType = Jkr::GUIPainterParameter<Jkr::GUIPainterParameterContext::UniformBuffer>;
	private:
		Up<StagingBuffer> mStagingVertexBuffer;
		Up<StagingBuffer> mStagingIndexBuffer;
		void* mVertexStagingBufferMemoryMapPtr = nullptr;
		void* mIndexStagingBufferMemoryMapPtr = nullptr;
		std::vector<vk::BufferCopy> mVertexCopyRegionsToBeSubmittedEachFrame;
		std::vector<vk::BufferCopy> mIndexCopyRegionsToBeSubmittedEachFrame;
		std::vector<vk::BufferCopy> mVertexCopyRegionsToBeSubmittedOnce;
		std::vector<vk::BufferCopy> mIndexCopyRegionsToBeSubmittedOnce;
	private:
		size_t mStagingVertexBufferSize = 0;
		size_t mStagingIndexBufferSize = 0;
	protected:
		void CreatePainter(const Instance& inInstance, GUIWindow& inCompatibleWindow, GUIPainterCache& inPainterCache);
		void CreateStagingBuffers(const Instance& inInstance, size_t inVertexStagingBufferSizeInBytes, size_t inIndexStagingBufferSizeInBytes);
		void CopyToStagingBuffers(void* inVertexData, void* inIndexData, size_t inVertexOffset, size_t inIndexOffset, size_t inVertexSize, size_t inIndexSize);
		void ResizeStagingBuffer(const Instance& inInstance, size_t inVertexStagingBufferSizeInBytes, size_t inIndexStagingBufferSizeInBytes);

		bool IsSingleTimeCopyRegionsEmpty() const { return mVertexCopyRegionsToBeSubmittedOnce.empty() && mIndexCopyRegionsToBeSubmittedOnce.empty(); }
		void CmdCopyToPrimitiveFromStagingBufferSingleTime(const Instance& inInstance, GUIPrimitive& inPrimitive, GUIWindow& inWindow, size_t inVertexMemorySizeToBeBarriered, size_t inIndexMemorySizeToBeBarriered);
		void CmdCopyToPrimitiveFromStagingBufferEachFrame(const Instance& inInstance, GUIPrimitive& inPrimitive, GUIWindow& inWindow, size_t inVertexMemorySizeToBeBarriered, size_t inIndexMemorySizeToBeBarriered);
		void RegisterBufferCopyRegionToPrimitiveFromStagingFirstFrameOnly(size_t inVertexOffset, size_t inIndexOffset, size_t inVertexSize, size_t inIndexSize);
		void RegisterBufferCopyRegionToPrimitiveFromStagingEachFrame(size_t inVertexOffset, size_t inIndexOffset, size_t inVertexSize, size_t inIndexSize);

		GETTER& GetVertexStagingBufferMemoryMapPtr() { return mVertexStagingBufferMemoryMapPtr; }
		GETTER& GetIndexStagingBufferMemoryMapPtr() { return mIndexStagingBufferMemoryMapPtr; }
		GETTER& GetVertexStagingBufferSize() { return mStagingVertexBufferSize; }
		GETTER& GetIndexStagingBufferSize() { return mStagingIndexBufferSize; }
	};
}
