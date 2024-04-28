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
    // TODO Create Separate Command Pool and Do these things
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
    template <BufferContext inBufferContext, MemoryType inBufferStorageType>
    constexpr void FillBufferUsage(vk::BufferCreateInfo& inInfo);
    void GetMemoryTypeIndex(vk::MemoryPropertyFlags inFlag,
                            vk::Buffer inBuffer,
                            vk::DeviceSize& outSize,
                            uint32_t& outIndex);
    vk::PhysicalDeviceMemoryProperties GetMemoryProperties() const;

    protected:
    const vk::Device& mDevice;
    const vk::PhysicalDevice& mPhysicalDevice;
    vk::Buffer mBufferHandle;
    vk::DeviceMemory mBufferMemory;
    vk::DeviceSize mSize;
};
} // namespace ksai

namespace ksai {
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

    public:
};
} // namespace ksai

namespace ksai {
template <BufferContext inBufferContext, MemoryType inBufferStorageType>
inline constexpr void VulkanBufferBase::FillBufferUsage(vk::BufferCreateInfo& inInfo) {
    if constexpr (inBufferContext == BufferContext::Vertex)
        inInfo.setUsage(vk::BufferUsageFlagBits::eVertexBuffer |
                        vk::BufferUsageFlagBits::eTransferDst);
    else if constexpr (inBufferContext == BufferContext::Index)
        inInfo.setUsage(vk::BufferUsageFlagBits::eIndexBuffer |
                        vk::BufferUsageFlagBits::eTransferDst);
    else if constexpr (inBufferContext == BufferContext::Uniform)
        inInfo.setUsage(vk::BufferUsageFlagBits::eUniformBuffer |
                        vk::BufferUsageFlagBits::eTransferDst);
    else if constexpr (inBufferContext == BufferContext::Storage)
        inInfo.setUsage(vk::BufferUsageFlagBits::eStorageBuffer |
                        vk::BufferUsageFlagBits::eTransferDst);
    else if constexpr (inBufferContext == (BufferContext::Vertex | BufferContext::Storage))
        inInfo.setUsage(vk::BufferUsageFlagBits::eStorageBuffer |
                        vk::BufferUsageFlagBits::eVertexBuffer |
                        vk::BufferUsageFlagBits::eTransferDst);
    else if constexpr (inBufferContext == BufferContext::Staging)
        inInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc);
    else if constexpr (inBufferContext == (BufferContext::Uniform | BufferContext::Storage))
        inInfo.setUsage(vk::BufferUsageFlagBits::eStorageBuffer |
                        vk::BufferUsageFlagBits::eUniformBuffer |
                        vk::BufferUsageFlagBits::eTransferDst);
}

template <BufferContext Context>
inline void VulkanBufferBase::Bind(const VulkanCommandBuffer& inCmdBuffer) const {
    if constexpr (Context == BufferContext::Vertex)
        inCmdBuffer.GetCommandBufferHandle().bindVertexBuffers(0, mBufferHandle, 0.0f);
    else if constexpr (Context == BufferContext::Index)
        inCmdBuffer.GetCommandBufferHandle().bindIndexBuffer(
             mBufferHandle, 0, vk::IndexType::eUint32);
}
} // namespace ksai

namespace ksai {
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

    if constexpr (inBufferContext == BufferContext::Staging)
        Usage = vk::BufferUsageFlagBits::eTransferSrc;

    auto BufferCreateInfo = vk::BufferCreateInfo(vk::BufferCreateFlags(), inSize, Usage);
    FillBufferUsage<inBufferContext, inBufferStorageType>(BufferCreateInfo);

    mBufferHandle = mDevice.createBuffer(BufferCreateInfo);

    uint32_t MemoryTypeIndex;
    vk::DeviceSize MemorySize;
    vk::MemoryPropertyFlags MemoryPropertyFlagBits;

    if constexpr (inBufferStorageType == MemoryType::DeviceLocal)
        MemoryPropertyFlagBits = vk::MemoryPropertyFlagBits::eDeviceLocal;
    else if constexpr (inBufferStorageType == MemoryType::HostVisibleAndCoherenet)
        MemoryPropertyFlagBits =
             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    GetMemoryTypeIndex(MemoryPropertyFlagBits, mBufferHandle, MemorySize, MemoryTypeIndex);
    mBufferMemory = mDevice.allocateMemory(vk::MemoryAllocateInfo(MemorySize, MemoryTypeIndex));
    mDevice.bindBufferMemory(mBufferHandle, mBufferMemory, 0);
}

template <BufferContext inBufferContext, MemoryType inBufferStorageType>
inline VulkanBuffer<inBufferContext, inBufferStorageType>::~VulkanBuffer() {
    if constexpr (inBufferStorageType == MemoryType::HostVisibleAndCoherenet)
        mDevice.unmapMemory(mBufferMemory);
    mDevice.waitIdle();
    mDevice.destroyBuffer(mBufferHandle);
    mDevice.freeMemory(mBufferMemory);
}

} // namespace ksai
