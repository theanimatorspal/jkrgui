#pragma once
#include "Instance.hpp"
#include <VulkanBufferVMA.hpp>
#include <Window.hpp>
#include <glm/glm.hpp>

namespace Jkr {
class Primitive {
    using CmdParam         = Window::ParameterContext;
    using VertexBufferType = VulkanBufferVMA;
    using IndexBufferType  = VulkanBufferVMA;

    public:
    GETTER &GetVertexBufferPtr() const { return mVertexBufferPtr; }
    GETTER &GetIndexBufferPtr() const { return mIndexBufferPtr; }
    GETTER GetIndexCount() const { return mIndexCount; }
    void SetIndexCount(uint32_t inIndex) { mIndexCount = inIndex; }
    void Bind(Instance &inInstance, const Window_base &inWindow, CmdParam inCmdParam);

    Primitive(Instance &inInstance);
    Primitive(Instance &inInstance,
              const vk::ArrayProxyNoTemporaries<kstd::Vertex> &inVertices,
              const vk::ArrayProxyNoTemporaries<kstd::ui32> &inIndices);
    Primitive(Instance &inInstance,
              const vk::ArrayProxyNoTemporaries<kstd::VertexEXT> &inVertices,
              const vk::ArrayProxyNoTemporaries<kstd::ui32> &inIndices,
              int inExtParameter_DoesNotDoAnything);
    Primitive(Instance &inInstance,
              vk::DeviceSize inVertexBufferSizeInBytes,
              vk::DeviceSize inIndexBufferSizeInBytes);
    ~Primitive();

    private:
    Instance &mInstance;
    uint32_t mIndexCount = 0;

#ifndef JKR_NO_STAGING_BUFFERS
    Up<VulkanBufferVMA> mVertexBufferPtr = nullptr;
    Up<VulkanBufferVMA> mIndexBufferPtr  = nullptr;
#else
    public:
    GETTER &GetVertexMemoryMapPtr() { return mVertexBufferMappedMemory; }
    GETTER &GetIndexMemoryMapPtr() { return mIndexBufferMappedMemory; }

    private:
    Up<VertexBufferType> mVertexBufferPtr = nullptr;
    Up<IndexBufferType> mIndexBufferPtr   = nullptr;
    void *mVertexBufferMappedMemory       = nullptr;
    void *mIndexBufferMappedMemory        = nullptr;
#endif
};
} // namespace Jkr
