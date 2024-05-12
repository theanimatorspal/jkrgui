#pragma once
#include "Instance.hpp"
#include <VulkanBufferVMA.hpp>
#include <glm/glm.hpp>

namespace Jkr {
class Primitive {
    using VertexBufferType = VulkanBufferVMA<BufferContext::Vertex | BufferContext::Storage,
                                             MemoryType::HostVisibleAndHostCached>;
    using IndexBufferType  = VulkanBufferVMA<BufferContext::Index | BufferContext::Storage,
                                             MemoryType::HostVisibleAndHostCached>;

    public:
    Primitive(const Instance& inInstance);
    Primitive(const Instance& inInstance,
              const vk::ArrayProxyNoTemporaries<kstd::Vertex>& inVertices,
              const vk::ArrayProxyNoTemporaries<kstd::ui32>& inIndices);
    Primitive(const Instance& inInstance,
              const vk::ArrayProxyNoTemporaries<kstd::VertexEXT>& inVertices,
              const vk::ArrayProxyNoTemporaries<kstd::ui32>& inIndices,
              int inExtParameter_DoesNotDoAnything);
    Primitive(const Instance& inInstance,
              vk::DeviceSize inVertexBufferSizeInBytes,
              vk::DeviceSize inIndexBufferSizeInBytes);
    ~Primitive();

    public:
    GETTER& GetVertexBufferPtr() const { return mVertexBufferPtr; }
    GETTER& GetIndexBufferPtr() const { return mIndexBufferPtr; }
    GETTER GetIndexCount() const { return mIndexCount; }
    void SetIndexCount(uint32_t inIndex) { mIndexCount = inIndex; }

    private:
    const Instance& mInstance;
    uint32_t mIndexCount = 0;

#ifndef JKR_NO_STAGING_BUFFERS
    Up<VulkanBufferVMA<BufferContext::Vertex, MemoryType::DeviceLocal>> mVertexBufferPtr = nullptr;
    Up<VulkanBufferVMA<BufferContext::Index, MemoryType::DeviceLocal>> mIndexBufferPtr   = nullptr;
#endif

#ifdef JKR_NO_STAGING_BUFFERS
    public:
    GETTER& GetVertexMemoryMapPtr() { return mVertexBufferMappedMemory; }
    GETTER& GetIndexMemoryMapPtr() { return mIndexBufferMappedMemory; }

    private:
    Up<VertexBufferType> mVertexBufferPtr = nullptr;
    Up<IndexBufferType> mIndexBufferPtr   = nullptr;
    void* mVertexBufferMappedMemory       = nullptr;
    void* mIndexBufferMappedMemory        = nullptr;
#endif
};
} // namespace Jkr
