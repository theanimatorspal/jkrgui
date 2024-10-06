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
    struct CreateInfo {
        const VulkanDevice *inDevice;
    };

    VulkanBufferBase() = default;
    ~VulkanBufferBase();
    VulkanBufferBase(VulkanBufferBase &other)             = delete;
    VulkanBufferBase &operator=(VulkanBufferBase &other)  = delete;
    VulkanBufferBase(VulkanBufferBase &&other)            = default;
    VulkanBufferBase &operator=(VulkanBufferBase &&other) = default;

    void Init(CreateInfo inCreateInfo);
    void Destroy();

    vk::Buffer operator()(VulkanBufferBase &) { return mBufferHandle; }
    VulkanBufferBase(const VulkanDevice &inDevice);
    void SubmitImmediateCmdCopyFrom(const VulkanQueue<QueueContext::Graphics> &inQueue,
                                    const VulkanCommandBuffer &inCmdBuffer,
                                    void *inData);
    ///@note internal mechanisms for handling image layout transitions are not effective so,
    /// this is required (the inLayout Parameter), for this to work, the inLayout parameter should
    /// be equal to the layout of the image it will have before the copy. After the copy, this
    /// function will restore the image layout to inLayout.
    void SubmitImmediateCmdCopyFromImage(const VulkanQueue<QueueContext::Graphics> &inQueue,
                                         const VulkanCommandBuffer &inCmdBuffer,
                                         const VulkanFence &inFence,
                                         VulkanImageBase &inImage,
                                         vk::ImageLayout inLayout = vk::ImageLayout::eGeneral,
                                         int inMipLevel           = 0,
                                         int inLayer              = 0,
                                         int inLayersToBeCopied   = 1,
                                         int inImageWidth         = -1,
                                         int inImageHeight        = -1);

    v<char> SubmitImmediateGetBufferToVector(const VulkanQueue<QueueContext::Graphics> &inQueue,
                                             VulkanBufferVMA &inStagingBuffer,
                                             const VulkanCommandBuffer &inCmdBuffer,
                                             const VulkanFence &inFence);

    void CmdCopyFrom(const VulkanCommandBuffer &inCmdBuffer,
                     VulkanBufferBase &inBuffer,
                     vk::DeviceSize FromBufferOffset,
                     vk::DeviceSize ToBufferOffset,
                     vk::DeviceSize inSizeToCopy);
    void SetBarrier(const VulkanCommandBuffer &inCmdBuffer,
                    vk::DeviceSize inDstBufferOffset,
                    vk::DeviceSize inSizeToCopy,
                    vk::AccessFlags inBeforeAccess,
                    vk::AccessFlags inAfterAccess,
                    vk::PipelineStageFlags inBeforeStages,
                    vk::PipelineStageFlags inAfterStages);
    void CmdCopyFromImage(
         const VulkanCommandBuffer &inCmdBuffer,
         const VulkanImageBase &inImage,
         vk::PipelineStageFlags inAfterStage = vk::PipelineStageFlagBits::eComputeShader,
         vk::AccessFlags inAfterAccessFlags  = vk::AccessFlagBits::eMemoryRead) const;
    GETTER GetBufferHandle() const { return mBufferHandle; }
    GETTER GetBufferSize() const { return mSize; }

    public:
    template <BufferContext Context> void Bind(const VulkanCommandBuffer &inCmdBuffer) const;

    protected:
    void FillBufferUsage(vk::BufferCreateInfo &inInfo,
                         BufferContext inBufferContext,
                         MemoryType inBufferStorageType);
    void GetMemoryTypeIndex(vk::MemoryPropertyFlags inFlag,
                            vk::Buffer inBuffer,
                            vk::DeviceSize &outSize,
                            uint32_t &outIndex);
    vk::PhysicalDeviceMemoryProperties GetMemoryProperties() const;

    protected:
    const VulkanDevice *mVulkanDevice;
    const vk::Device *mDevice;
    const vk::PhysicalDevice *mPhysicalDevice;
    vk::Buffer mBufferHandle;
    vk::DeviceMemory mBufferMemory;
    vk::DeviceSize mSize;
    bool mInitialized = false;
};

class VulkanBuffer : public VulkanBufferBase {
    public:
    struct CreateInfo {
        const VulkanDevice *inDevice;
        size_t inSize;
        BufferContext inBufferContext = BufferContext::None;
        MemoryType inBufferMemoryType = MemoryType::DeviceLocal;
    };

    VulkanBuffer() = default;
    ~VulkanBuffer();
    VulkanBuffer(VulkanBuffer &other)             = delete;
    VulkanBuffer &operator=(VulkanBuffer &other)  = delete;
    VulkanBuffer(VulkanBuffer &&other)            = default;
    VulkanBuffer &operator=(VulkanBuffer &&other) = default;

    void Init(CreateInfo inCreateInfo);
    void Destroy();
    void MapMemoryRegion(void **outMappedMemoryRegion);
    void UnMapMemoryRegion();

    VulkanBuffer(const VulkanDevice &inDevice,
                 size_t inSize,
                 BufferContext inBufferContext,
                 MemoryType inBufferStorageType);

    private:
    bool mIsMemoryMapped = false;
    bool mInitialized    = false;
    void *mMappedMemoryRegion;
};

template <BufferContext Context>
inline void VulkanBufferBase::Bind(const VulkanCommandBuffer &inCmdBuffer) const {
    if (Context == BufferContext::Vertex)
        inCmdBuffer.GetCommandBufferHandle().bindVertexBuffers(0, mBufferHandle, 0.0f);
    else if (Context == BufferContext::Index)
        inCmdBuffer.GetCommandBufferHandle().bindIndexBuffer(
             mBufferHandle, 0, vk::IndexType::eUint32);
}

} // namespace ksai
