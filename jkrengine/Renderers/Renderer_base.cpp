#include "Renderer_base.hpp"

#ifndef JKR_NO_STAGING_BUFFERS
void Jkr::Renderer::Renderer_base::CreateStagingBuffers(Instance &inInstance,
                                                        size_t inVertexStagingBufferSizeInBytes,
                                                        size_t inIndexStagingBufferSizeInBytes) {
    mStagingVertexBuffer = MakeUp<StagingBuffer>(inInstance.GetVMA(),
                                                 inInstance.GetDevice(),
                                                 inVertexStagingBufferSizeInBytes,
                                                 ksai::BufferContext::Staging,
                                                 ksai::MemoryType::HostVisibleAndHostCached);
    mStagingVertexBuffer->MapMemoryRegion(&mVertexStagingBufferMemoryMapPtr);
    mStagingIndexBuffer = MakeUp<StagingBuffer>(
         inInstance.GetVMA(), inInstance.GetDevice(), inIndexStagingBufferSizeInBytes);
    mStagingIndexBuffer->MapMemoryRegion(&mIndexStagingBufferMemoryMapPtr);
    std::memset(mVertexStagingBufferMemoryMapPtr, 0, inVertexStagingBufferSizeInBytes);
    std::memset(mIndexStagingBufferMemoryMapPtr, 0, inIndexStagingBufferSizeInBytes);
    mStagingVertexBufferSize = inVertexStagingBufferSizeInBytes;
    mStagingIndexBufferSize  = inIndexStagingBufferSizeInBytes;
}

void Jkr::Renderer::Renderer_base::CopyToStagingBuffers(void *inVertexData,
                                                        void *inIndexData,
                                                        size_t inVertexOffset,
                                                        size_t inIndexOffset,
                                                        size_t inVertexSize,
                                                        size_t inIndexSize) {
    std::memcpy((char *)mVertexStagingBufferMemoryMapPtr + inVertexOffset,
                (char *)inVertexData + inVertexOffset,
                inVertexSize);
    mStagingVertexBuffer->FlushMemoryRanges(inVertexOffset, inVertexSize);

    std::memcpy((char *)mIndexStagingBufferMemoryMapPtr + inIndexOffset,
                (char *)inIndexData + inIndexOffset,
                inIndexSize);
    mStagingIndexBuffer->FlushMemoryRanges(inIndexOffset, inIndexSize);
}

void Jkr::Renderer::Renderer_base::ResizeStagingBuffer(Instance &inInstance,
                                                       size_t inVertexStagingBufferSizeInBytes,
                                                       size_t inIndexStagingBufferSizeInBytes) {
    mStagingVertexBuffer.reset();
    mStagingIndexBuffer.reset();
    CreateStagingBuffers(
         inInstance, inVertexStagingBufferSizeInBytes, inIndexStagingBufferSizeInBytes);
}

void Jkr::Renderer::Renderer_base::CmdCopyToPrimitiveFromStagingBuffer(
     Instance &inInstance,
     Primitive &inPrimitive,
     Window &inWindow,
     size_t inVertexMemorySizeToBeBarriered,
     size_t inIndexMemorySizeToBeBarriered) {
    inWindow.GetUtilCommandBuffer().GetCommandBufferHandle().copyBuffer(
         mStagingVertexBuffer->GetBufferHandle(),
         inPrimitive.GetVertexBufferPtr()->GetBufferHandle(),
         mVertexCopyRegionsToBeSubmitted);

    inPrimitive.GetVertexBufferPtr()->SetBarrier(inWindow.GetUtilCommandBuffer(),
                                                 0,
                                                 inVertexMemorySizeToBeBarriered,
                                                 vk::AccessFlagBits::eMemoryWrite,
                                                 vk::AccessFlagBits::eMemoryRead,
                                                 vk::PipelineStageFlagBits::eTransfer,
                                                 vk::PipelineStageFlagBits::eVertexInput);

    inWindow.GetUtilCommandBuffer().GetCommandBufferHandle().copyBuffer(
         mStagingIndexBuffer->GetBufferHandle(),
         inPrimitive.GetIndexBufferPtr()->GetBufferHandle(),
         mIndexCopyRegionsToBeSubmitted);

    inPrimitive.GetIndexBufferPtr()->SetBarrier(inWindow.GetUtilCommandBuffer(),
                                                0,
                                                inIndexMemorySizeToBeBarriered,
                                                vk::AccessFlagBits::eMemoryWrite,
                                                vk::AccessFlagBits::eMemoryRead,
                                                vk::PipelineStageFlagBits::eTransfer,
                                                vk::PipelineStageFlagBits::eVertexInput);
    mVertexCopyRegionsToBeSubmitted.clear();
    mIndexCopyRegionsToBeSubmitted.clear();
}

void Jkr::Renderer::Renderer_base::RegisterBufferCopyRegionToPrimitiveFromStaging(
     size_t inVertexOffset, size_t inIndexOffset, size_t inVertexSize, size_t inIndexSize) {
    mVertexCopyRegionsToBeSubmitted.push_back(
         vk::BufferCopy(inVertexOffset, inVertexOffset, inVertexSize));
    mIndexCopyRegionsToBeSubmitted.push_back(
         vk::BufferCopy(inIndexOffset, inIndexOffset, inIndexSize));
}

#else
void Jkr::Renderer::Renderer_base::CopyToPrimitive(Primitive &inPrimitive,
                                                   void *inVertexData,
                                                   void *inIndexData,
                                                   size_t inVertexOffset,
                                                   size_t inIndexOffset,
                                                   size_t inVertexSize,
                                                   size_t inIndexSize) {
    std::scoped_lock<std::mutex> Lock(mCopyMutex);
    std::memcpy((char *)inPrimitive.GetVertexMemoryMapPtr() + inVertexOffset,
                (char *)inVertexData + inVertexOffset,
                inVertexSize);
    inPrimitive.GetVertexBufferPtr()->FlushMemoryRanges(inVertexOffset, inVertexSize);

    std::memcpy((char *)inPrimitive.GetIndexMemoryMapPtr() + inIndexOffset,
                (char *)inIndexData + inIndexOffset,
                inIndexSize);
    inPrimitive.GetIndexBufferPtr()->FlushMemoryRanges(inIndexOffset, inIndexSize);
}
#endif
