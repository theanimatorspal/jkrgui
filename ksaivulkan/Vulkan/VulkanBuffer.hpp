#pragma once
#include "VulkanCommandBuffer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanQueue.hpp"
#include "vulkan/vulkan_enums.hpp"

namespace ksai {
enum class BufferContext { None, Vertex, Index, Storage, Uniform, Staging };

inline constexpr BufferContext operator|(BufferContext lhs, BufferContext rhs) {
    using T = std::underlying_type_t<BufferContext>;
    return static_cast<BufferContext>(static_cast<T>(lhs) | static_cast<T>(rhs));
}
} // namespace ksai

namespace ksai {
class VulkanBufferBase {
    public:
    VulkanBufferBase(const VulkanDevice& inDevice);
    virtual ~VulkanBufferBase() = default;
    void SubmitImmediateCmdCopyFrom(const VulkanQueue<QueueContext::Graphics>& inQueue,
                                    const VulkanCommandBuffer& inCmdBuffer,
                                    void* inData);
    void SubmitImmediateCmdCopyFromImage(const VulkanQueue<QueueContext::Graphics>& inQueue,
                                         const VulkanCommandBuffer& inCmdBuffer,
                                         const VulkanImageBase& inImage) const;

    void CmdCopyFrom(const VulkanCommandBuffer& inCmdBuffer,
                     VulkanBufferBase& inBuffer,
                     vk::DeviceSize FromBufferOffset,
                     vk::DeviceSize ToBufferOffset,
                     vk::DeviceSize inSizeToCopy);
    void SetBarrier(const VulkanCommandBuffer& inCmdBuffer,
                    vk::DeviceSize inDstBufferOffset,
                    vk::DeviceSize inSizeToCopy,
                    vk::AccessFlags inBeforeAccess,
                    vk::AccessFlags inAfterAccess,
                    vk::PipelineStageFlags inBeforeStages,
                    vk::PipelineStageFlags inAfterStages);
    void CmdCopyFromImage(
         const VulkanCommandBuffer& inCmdBuffer,
         const VulkanImageBase& inImage,
         vk::PipelineStageFlags inAfterStage = vk::PipelineStageFlagBits::eComputeShader,
         vk::AccessFlags inAfterAccessFlags  = vk::AccessFlagBits::eMemoryRead) const;
    GETTER GetBufferHandle() const { return mBufferHandle; }
    GETTER GetBufferSize() const { return mSize; }

    public:
    template <BufferContext Context> void Bind(const VulkanCommandBuffer& inCmdBuffer) const;

    protected:
    void FillBufferUsage(vk::BufferCreateInfo& inInfo,
                         BufferContext inBufferContext,
                         MemoryType inBufferStorageType);
    void GetMemoryTypeIndex(vk::MemoryPropertyFlags inFlag,
                            vk::Buffer inBuffer,
                            vk::DeviceSize& outSize,
                            uint32_t& outIndex);
    vk::PhysicalDeviceMemoryProperties GetMemoryProperties() const;

    protected:
    const VulkanDevice& mVulkanDevice;
    const vk::Device& mDevice;
    const vk::PhysicalDevice& mPhysicalDevice;
    vk::Buffer mBufferHandle;
    vk::DeviceMemory mBufferMemory;
    vk::DeviceSize mSize;
};

template <BufferContext inBufferContext, MemoryType inBufferStorageType>
class VulkanBuffer : public VulkanBufferBase {
    public:
    constexpr void MapMemoryRegion(void** outMappedMemoryRegion)
        requires(inBufferStorageType == MemoryType::HostVisibleAndCoherenet);
    constexpr void UnMapMemoryRegion()
        requires(inBufferStorageType == MemoryType::HostVisibleAndCoherenet);

    public:
    VulkanBuffer(const VulkanDevice& inDevice, size_t inSize);
    ~VulkanBuffer();
};

template <BufferContext Context>
inline void VulkanBufferBase::Bind(const VulkanCommandBuffer& inCmdBuffer) const {
    if (Context == BufferContext::Vertex)
        inCmdBuffer.GetCommandBufferHandle().bindVertexBuffers(0, mBufferHandle, 0.0f);
    else if (Context == BufferContext::Index)
        inCmdBuffer.GetCommandBufferHandle().bindIndexBuffer(
             mBufferHandle, 0, vk::IndexType::eUint32);
}

template <BufferContext inBufferContext, MemoryType inBufferStorageType>
inline constexpr void
VulkanBuffer<inBufferContext, inBufferStorageType>::MapMemoryRegion(void** outMappedMemoryRegion)
    requires(inBufferStorageType == MemoryType::HostVisibleAndCoherenet)
{
    *outMappedMemoryRegion =
         static_cast<void*>(mDevice.mapMemory(mBufferMemory, 0, GetBufferSize()));
}
template <BufferContext inBufferContext, MemoryType inBufferStorageType>
inline constexpr void VulkanBuffer<inBufferContext, inBufferStorageType>::UnMapMemoryRegion()
    requires(inBufferStorageType == MemoryType::HostVisibleAndCoherenet)
{
    mDevice.unmapMemory(mBufferMemory);
}
template <BufferContext inBufferContext, MemoryType inBufferStorageType>
inline VulkanBuffer<inBufferContext, inBufferStorageType>::VulkanBuffer(
     const VulkanDevice& inDevice, size_t inSize)
    : VulkanBufferBase(inDevice) {
    mSize      = inSize;
    auto Usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;

    if (inBufferContext == BufferContext::Staging) Usage = vk::BufferUsageFlagBits::eTransferSrc;

    auto BufferCreateInfo = vk::BufferCreateInfo(vk::BufferCreateFlags(), inSize, Usage);
    FillBufferUsage(BufferCreateInfo, inBufferContext, inBufferStorageType);

    mBufferHandle = mDevice.createBuffer(BufferCreateInfo);

    uint32_t MemoryTypeIndex;
    vk::DeviceSize MemorySize;
    vk::MemoryPropertyFlags MemoryPropertyFlagBits;

    if (inBufferStorageType == MemoryType::DeviceLocal)
        MemoryPropertyFlagBits = vk::MemoryPropertyFlagBits::eDeviceLocal;
    else if (inBufferStorageType == MemoryType::HostVisibleAndCoherenet)
        MemoryPropertyFlagBits =
             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    GetMemoryTypeIndex(MemoryPropertyFlagBits, mBufferHandle, MemorySize, MemoryTypeIndex);
    mBufferMemory = mDevice.allocateMemory(vk::MemoryAllocateInfo(MemorySize, MemoryTypeIndex));
    mDevice.bindBufferMemory(mBufferHandle, mBufferMemory, 0);
}

template <BufferContext inBufferContext, MemoryType inBufferStorageType>
inline VulkanBuffer<inBufferContext, inBufferStorageType>::~VulkanBuffer() {
    if (inBufferStorageType == MemoryType::HostVisibleAndCoherenet) {
        mDevice.unmapMemory(mBufferMemory);
    }
    mVulkanDevice.Wait();
    mDevice.destroyBuffer(mBufferHandle);
    mDevice.freeMemory(mBufferMemory);
}

} // namespace ksai
