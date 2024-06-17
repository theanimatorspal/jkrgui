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

class VulkanBufferBase {
    public:
    vk::Buffer operator()(const VulkanBufferBase&) { return mBufferHandle; }
    VulkanBufferBase(const VulkanDevice& inDevice);
    virtual ~VulkanBufferBase() = default;
    void SubmitImmediateCmdCopyFrom(const VulkanQueue<QueueContext::Graphics>& inQueue,
                                    const VulkanCommandBuffer& inCmdBuffer,
                                    void* inData);
    void SubmitImmediateCmdCopyFromImage(const VulkanQueue<QueueContext::Graphics>& inQueue,
                                         const VulkanCommandBuffer& inCmdBuffer,
                                         VulkanImageBase& inImage) const;

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

class VulkanBuffer : public VulkanBufferBase {
    public:
    vk::Buffer operator()(const VulkanBuffer&) { return mBufferHandle; }
    void MapMemoryRegion(void** outMappedMemoryRegion);
    void UnMapMemoryRegion();

    VulkanBuffer(const VulkanDevice& inDevice,
                 size_t inSize,
                 BufferContext inBufferContext,
                 MemoryType inBufferStorageType);
    ~VulkanBuffer();

    private:
    bool mIsMemoryMapped = false;
};

template <BufferContext Context>
inline void VulkanBufferBase::Bind(const VulkanCommandBuffer& inCmdBuffer) const {
    if (Context == BufferContext::Vertex)
        inCmdBuffer.GetCommandBufferHandle().bindVertexBuffers(0, mBufferHandle, 0.0f);
    else if (Context == BufferContext::Index)
        inCmdBuffer.GetCommandBufferHandle().bindIndexBuffer(
             mBufferHandle, 0, vk::IndexType::eUint32);
}

} // namespace ksai
