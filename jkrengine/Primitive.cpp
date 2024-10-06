#include "Primitive.hpp"

// TODO Make these thread safe
/// @warning There are a lot of bad codes here
/// @todo fix this on refactor pass later
static std::mutex PriMutex;

Jkr::Primitive::Primitive(Instance &inInstance) : mInstance(inInstance) {}
Jkr::Primitive::~Primitive() { std::scoped_lock<std::mutex> Lock(PriMutex); }

#ifndef JKR_NO_STAGING_BUFFERS
Jkr::Primitive::Primitive(Instance &inInstance,
                          const vk::ArrayProxyNoTemporaries<kstd::Vertex> &inVertices,
                          const vk::ArrayProxyNoTemporaries<kstd::ui32> &inIndices)
    : mInstance(inInstance), mIndexCount(inIndices.size()) {
    std::scoped_lock<std::mutex> Lock(PriMutex);
    mVertexBufferPtr =
         MakeUp<VertexBufferType>(inInstance.GetVMA(),
                                  mInstance.GetDevice(),
                                  vk::DeviceSize(sizeof(inVertices.front()) * inVertices.size()),
                                  BufferContext::Vertex,
                                  MemoryType::HostVisibleAndHostCached);
    mIndexBufferPtr = MakeUp<IndexBufferType>(inInstance.GetVMA(),
                                              mInstance.GetDevice(),
                                              vk::DeviceSize(sizeof(uint32_t) * inIndices.size()),
                                              BufferContext::Index,
                                              MemoryType::HostVisibleAndHostCached);
    mVertexBufferPtr->SubmitImmediateCmdCopyFrom(mInstance.GetGraphicsQueue(),
                                                 mInstance.GetUtilCommandBuffer(),
                                                 reinterpret_cast<void *>(inVertices.data()));
    mIndexBufferPtr->SubmitImmediateCmdCopyFrom(mInstance.GetGraphicsQueue(),
                                                mInstance.GetUtilCommandBuffer(),
                                                reinterpret_cast<void *>(inIndices.data()));
    mInstance.GetGraphicsQueue().Wait();
}

Jkr::Primitive::Primitive(Instance &inInstance,
                          const vk::ArrayProxyNoTemporaries<kstd::VertexEXT> &inVertices,
                          const vk::ArrayProxyNoTemporaries<kstd::ui32> &inIndices,
                          int inExtParameter_DoesNotDoAnything)
    : mInstance(inInstance), mIndexCount(inIndices.size()) {
    std::scoped_lock<std::mutex> Lock(PriMutex);
    mVertexBufferPtr =
         MakeUp<VertexBufferType>(inInstance.GetVMA(),
                                  mInstance.GetDevice(),
                                  vk::DeviceSize(sizeof(inVertices.front()) * inVertices.size()),
                                  BufferContext::Vertex,
                                  MemoryType::HostVisibleAndHostCached);
    mIndexBufferPtr = MakeUp<IndexBufferType>(inInstance.GetVMA(),
                                              mInstance.GetDevice(),
                                              vk::DeviceSize(sizeof(uint32_t) * inIndices.size()),
                                              BufferContext::Index,
                                              MemoryType::HostVisibleAndHostCached);
    mVertexBufferPtr->SubmitImmediateCmdCopyFrom(mInstance.GetGraphicsQueue(),
                                                 mInstance.GetUtilCommandBuffer(),
                                                 reinterpret_cast<void *>(inVertices.data()));
    mIndexBufferPtr->SubmitImmediateCmdCopyFrom(mInstance.GetGraphicsQueue(),
                                                mInstance.GetUtilCommandBuffer(),
                                                reinterpret_cast<void *>(inIndices.data()));
    mInstance.GetGraphicsQueue().Wait();
}

Jkr::Primitive::Primitive(Instance &inInstance,
                          vk::DeviceSize inVertexBufferSizeInBytes,
                          vk::DeviceSize inIndexBufferSizeInBytes)
    : mInstance(inInstance), mIndexCount(0) {
    std::scoped_lock<std::mutex> Lock(PriMutex);
    mVertexBufferPtr = MakeUp < VulkanBufferVMA<VertexBufferType>(inInstance.GetVMA(),
                                                                  mInstance.GetDevice(),
                                                                  inVertexBufferSizeInBytes);
    mIndexBufferPtr =
         MakeUp <
         IndexBufferType(inInstance.GetVMA(), mInstance.GetDevice(), inIndexBufferSizeInBytes);
}
#else
Jkr::Primitive::Primitive(Instance &inInstance,
                          const vk::ArrayProxyNoTemporaries<kstd::Vertex> &inVertices,
                          const vk::ArrayProxyNoTemporaries<kstd::ui32> &inIndices)
    : mInstance(inInstance), mIndexCount(inIndices.size()) {
    std::scoped_lock<std::mutex> Lock(PriMutex);
    mVertexBufferPtr =
         MakeUp<VertexBufferType>(inInstance.GetVMA(),
                                  mInstance.GetDevice(),
                                  vk::DeviceSize(sizeof(inVertices.front()) * inVertices.size()),
                                  BufferContext::Vertex,
                                  MemoryType::HostVisibleAndHostCached);
    mIndexBufferPtr = MakeUp<IndexBufferType>(inInstance.GetVMA(),
                                              mInstance.GetDevice(),
                                              vk::DeviceSize(sizeof(uint32_t) * inIndices.size()),
                                              BufferContext::Index,
                                              MemoryType::HostVisibleAndHostCached);
    mVertexBufferPtr->SubmitImmediateCmdCopyFrom(mInstance.GetGraphicsQueue(),
                                                 mInstance.GetUtilCommandBuffer(),
                                                 reinterpret_cast<void *>(inVertices.data()));
    mIndexBufferPtr->SubmitImmediateCmdCopyFrom(mInstance.GetGraphicsQueue(),
                                                mInstance.GetUtilCommandBuffer(),
                                                reinterpret_cast<void *>(inIndices.data()));
    mInstance.GetGraphicsQueue().Wait();
    mVertexBufferPtr->MapMemoryRegion(&mVertexBufferMappedMemory);
    mIndexBufferPtr->MapMemoryRegion(&mIndexBufferMappedMemory);
}

Jkr::Primitive::Primitive(Instance &inInstance,
                          const vk::ArrayProxyNoTemporaries<kstd::VertexEXT> &inVertices,
                          const vk::ArrayProxyNoTemporaries<kstd::ui32> &inIndices,
                          int inExtParameter_DoesNotDoAnything)
    : mInstance(inInstance), mIndexCount(inIndices.size()) {
    std::scoped_lock<std::mutex> Lock(PriMutex);
    mVertexBufferPtr =
         MakeUp<VertexBufferType>(inInstance.GetVMA(),
                                  mInstance.GetDevice(),
                                  vk::DeviceSize(sizeof(inVertices.front()) * inVertices.size()),
                                  BufferContext::Vertex,
                                  MemoryType::HostVisibleAndHostCached);
    mIndexBufferPtr = MakeUp<IndexBufferType>(inInstance.GetVMA(),
                                              mInstance.GetDevice(),
                                              vk::DeviceSize(sizeof(uint32_t) * inIndices.size()),
                                              BufferContext::Index,
                                              MemoryType::HostVisibleAndHostCached);
    mVertexBufferPtr->SubmitImmediateCmdCopyFrom(mInstance.GetGraphicsQueue(),
                                                 mInstance.GetUtilCommandBuffer(),
                                                 reinterpret_cast<void *>(inVertices.data()));
    mIndexBufferPtr->SubmitImmediateCmdCopyFrom(mInstance.GetGraphicsQueue(),
                                                mInstance.GetUtilCommandBuffer(),
                                                reinterpret_cast<void *>(inIndices.data()));
    mInstance.GetGraphicsQueue().Wait();
    mVertexBufferPtr->MapMemoryRegion(&mVertexBufferMappedMemory);
    mIndexBufferPtr->MapMemoryRegion(&mIndexBufferMappedMemory);
}

Jkr::Primitive::Primitive(Instance &inInstance,
                          vk::DeviceSize inVertexBufferSizeInBytes,
                          vk::DeviceSize inIndexBufferSizeInBytes)
    : mInstance(inInstance), mIndexCount(0) {
    std::scoped_lock<std::mutex> Lock(PriMutex);
    mVertexBufferPtr = MakeUp<VertexBufferType>(inInstance.GetVMA(),
                                                mInstance.GetDevice(),
                                                inVertexBufferSizeInBytes,
                                                BufferContext::Vertex,
                                                MemoryType::HostVisibleAndHostCached);
    mIndexBufferPtr  = MakeUp<IndexBufferType>(inInstance.GetVMA(),
                                              mInstance.GetDevice(),
                                              inIndexBufferSizeInBytes,
                                              BufferContext::Index,
                                              MemoryType::HostVisibleAndHostCached);
    mVertexBufferPtr->MapMemoryRegion(&mVertexBufferMappedMemory);
    mIndexBufferPtr->MapMemoryRegion(&mIndexBufferMappedMemory);
}
#endif

void Jkr::Primitive::Bind(Instance &inInstance, const Window_base &inWindow, CmdParam inCmdParam) {
    auto &Cmd = inWindow.GetCommandBuffers(inCmdParam)[inWindow.GetCurrentFrame()];
    GetVertexBufferPtr()->Bind<BufferContext::Vertex>(Cmd);
    GetIndexBufferPtr()->Bind<BufferContext::Index>(Cmd);
}