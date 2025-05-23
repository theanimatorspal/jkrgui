#pragma once
#include "Painter.hpp"
#include "PainterParameter.hpp"
#include "Window_base.hpp"
#include <Vulkan/VulkanBufferVMA.hpp>

namespace Jkr::Renderer {

using CmdParam = Window_base::ParameterContext;
class Renderer_base {
    public:
    using ImageType        = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformImage>;
    using StorageImageType = Jkr::PainterParameter<Jkr::PainterParameterContext::StorageImage>;
    using SamplerType      = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformSampler>;
    using UniformType      = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformBuffer>;
    const ui InitialRendererElementArraySize = 2;
    const ui RendererCapacityResizeFactor    = 2;

    public:
    using StagingBuffer = Jkr::VulkanBufferVMA;
#ifndef JKR_NO_STAGING_BUFFERS
    private:
    Up<StagingBuffer> mStagingVertexBuffer;
    Up<StagingBuffer> mStagingIndexBuffer;
    void *mVertexStagingBufferMemoryMapPtr = nullptr;
    void *mIndexStagingBufferMemoryMapPtr  = nullptr;
    std::vector<vk::BufferCopy> mVertexCopyRegionsToBeSubmitted;
    std::vector<vk::BufferCopy> mIndexCopyRegionsToBeSubmitted;

    private:
    size_t mStagingVertexBufferSize = 0;
    size_t mStagingIndexBufferSize  = 0;

    protected:
#endif
    void CreatePainter(Instance &inInstance,
                       Window_base &inCompatibleWindow,
                       PainterCache &inPainterCache);
#ifndef JKR_NO_STAGING_BUFFERS
    void CreateStagingBuffers(Instance &inInstance,
                              size_t inVertexStagingBufferSizeInBytes,
                              size_t inIndexStagingBufferSizeInBytes);
    void CopyToStagingBuffers(void *inVertexData,
                              void *inIndexData,
                              size_t inVertexOffset,
                              size_t inIndexOffset,
                              size_t inVertexSize,
                              size_t inIndexSize);
    void ResizeStagingBuffer(Instance &inInstance,
                             size_t inVertexStagingBufferSizeInBytes,
                             size_t inIndexStagingBufferSizeInBytes);

    bool IsCopyRegionsEmpty() const {
        return mVertexCopyRegionsToBeSubmitted.empty() && mIndexCopyRegionsToBeSubmitted.empty();
    }
    void CmdCopyToPrimitiveFromStagingBuffer(Instance &inInstance,
                                             Primitive &inPrimitive,
                                             Window &inWindow,
                                             size_t inVertexMemorySizeToBeBarriered,
                                             size_t inIndexMemorySizeToBeBarriered);
    void RegisterBufferCopyRegionToPrimitiveFromStaging(size_t inVertexOffset,
                                                        size_t inIndexOffset,
                                                        size_t inVertexSize,
                                                        size_t inIndexSize);

    GETTER &GetVertexStagingBufferMemoryMapPtr() { return mVertexStagingBufferMemoryMapPtr; }
    GETTER &GetIndexStagingBufferMemoryMapPtr() { return mIndexStagingBufferMemoryMapPtr; }
    GETTER &GetVertexStagingBufferSize() { return mStagingVertexBufferSize; }
    GETTER &GetIndexStagingBufferSize() { return mStagingIndexBufferSize; }
#endif

#ifdef JKR_NO_STAGING_BUFFERS
    void CopyToPrimitive(Primitive &inPrimitive,
                         void *inVertexData,
                         void *inIndexData,
                         size_t inVertexOffset,
                         size_t inIndexOffset,
                         size_t inVertexSize,
                         size_t inIndexSize);

    private:
    std::mutex mCopyMutex;
#endif
};
} // namespace Jkr::Renderer
