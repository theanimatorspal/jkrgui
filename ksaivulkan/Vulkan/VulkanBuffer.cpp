#include "VulkanBuffer.hpp"

using namespace ksai;

ksai::VulkanBufferBase::VulkanBufferBase(const VulkanDevice &inDevice) { Init({&inDevice}); }

void VulkanBufferBase::Init(CreateInfo inCreateInfo) {
    mDevice         = &inCreateInfo.inDevice->GetDeviceHandle();
    mVulkanDevice   = inCreateInfo.inDevice;
    mPhysicalDevice = &inCreateInfo.inDevice->GetPhysicalDeviceHandle();
    mInitialized    = true;
}
void VulkanBufferBase::Destroy() { mInitialized = false; }

VulkanBufferBase::~VulkanBufferBase() {
    if (mInitialized) Destroy();
};

void ksai::VulkanBufferBase::SubmitImmediateCmdCopyFrom(
     const VulkanQueue<QueueContext::Graphics> &inQueue,
     const VulkanCommandBuffer &inCmdBuffer,
     void *inData) {
    uint32_t MemoryIndex;
    vk::DeviceSize MemorySize;
    GetMemoryTypeIndex(vk::MemoryPropertyFlagBits::eHostVisible |
                            vk::MemoryPropertyFlagBits::eHostCoherent,
                       mBufferHandle,
                       MemorySize,
                       MemoryIndex);
    auto StagingBufferCreateInfo = vk::BufferCreateInfo(
         vk::BufferCreateFlags(), MemorySize, vk::BufferUsageFlagBits::eTransferSrc);
    auto StagingBuffer = mDevice->createBuffer(StagingBufferCreateInfo);
    auto StagingBufferMemory =
         mDevice->allocateMemory(vk::MemoryAllocateInfo(MemorySize, MemoryIndex));
    mDevice->bindBufferMemory(StagingBuffer, StagingBufferMemory, 0);
    uint8_t *pData = static_cast<uint8_t *>(mDevice->mapMemory(StagingBufferMemory, 0, MemorySize));
    std::memcpy(pData, inData, MemorySize);
    mDevice->unmapMemory(StagingBufferMemory);
    const vk::CommandBuffer &Cmd = inCmdBuffer.GetCommandBufferHandle();
    Cmd.begin(vk::CommandBufferBeginInfo());
    std::array<vk::BufferCopy, 1> Regions = {vk::BufferCopy(0, 0, MemorySize)};
    inCmdBuffer.GetCommandBufferHandle().copyBuffer(StagingBuffer, mBufferHandle, Regions);
    Cmd.end();
    inQueue.Submit<SubmitContext::SingleTime>(inCmdBuffer);
    mDevice->freeMemory(StagingBufferMemory);
    mDevice->destroyBuffer(StagingBuffer);
}

void ksai::VulkanBufferBase::CmdCopyFrom(const VulkanCommandBuffer &inCmdBuffer,
                                         VulkanBufferBase &inBuffer,
                                         vk::DeviceSize FromBufferOffset,
                                         vk::DeviceSize ToBufferOffset,
                                         vk::DeviceSize inSizeToCopy) {
    vk::BufferCopy BufferCopyRegion =
         vk::BufferCopy(FromBufferOffset, ToBufferOffset, inSizeToCopy);
    inCmdBuffer.GetCommandBufferHandle().copyBuffer(
         inBuffer.mBufferHandle, mBufferHandle, BufferCopyRegion);
}

void ksai::VulkanBufferBase::SetBarrier(const VulkanCommandBuffer &inCmdBuffer,
                                        vk::DeviceSize inDstBufferOffset,
                                        vk::DeviceSize inSizeToCopy,
                                        vk::AccessFlags inBeforeAccess,
                                        vk::AccessFlags inAfterAccess,
                                        vk::PipelineStageFlags inBeforeStages,
                                        vk::PipelineStageFlags inAfterStages) {
    vk::BufferMemoryBarrier Barrier(inBeforeAccess,
                                    inAfterAccess,
                                    VK_QUEUE_FAMILY_IGNORED,
                                    VK_QUEUE_FAMILY_IGNORED,
                                    mBufferHandle,
                                    inDstBufferOffset,
                                    inSizeToCopy);
    inCmdBuffer.GetCommandBufferHandle().pipelineBarrier(
         inBeforeStages, inAfterStages, vk::DependencyFlagBits::eByRegion, {}, Barrier, {});
}

void ksai::VulkanBufferBase::GetMemoryTypeIndex(vk::MemoryPropertyFlags inFlag,
                                                vk::Buffer inBuffer,
                                                vk::DeviceSize &outSize,
                                                uint32_t &outIndex) {
    vk::PhysicalDeviceMemoryProperties memoryProperties = mPhysicalDevice->getMemoryProperties();
    vk::MemoryRequirements memoryRequirements = mDevice->getBufferMemoryRequirements(inBuffer);
    uint32_t typeBits                         = memoryRequirements.memoryTypeBits;
    uint32_t typeIndex                        = uint32_t(~0);
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if ((typeBits & 1) &&
            ((memoryProperties.memoryTypes[i].propertyFlags & inFlag) == inFlag)) {
            typeIndex = i;
            outIndex  = typeIndex;
            outSize   = memoryRequirements.size;
            return;
        }
        typeBits >>= 1;
    }
}

void ksai::VulkanBufferBase::CmdCopyFromImage(const VulkanCommandBuffer &inCmdBuffer,
                                              const VulkanImageBase &inImage,
                                              vk::PipelineStageFlags inAfterStage,
                                              vk::AccessFlags inAfterAccessFlags) const {
    // TODO
}

void ksai::VulkanBufferBase::SubmitImmediateCmdCopyFromImage(
     const VulkanQueue<QueueContext::Graphics> &inQueue,
     const VulkanCommandBuffer &inCmdBuffer,
     VulkanImageBase &inImage,
     vk::ImageLayout inLayout,
     int inMipLevel,
     int inLayer,
     int inLayersToBeCopied,
     int inImageWidth,
     int inImageHeight) const {
    auto ImageExtent = inImage.GetImageExtent();
    if (inImageWidth > 0) {
        ImageExtent.width = inImageWidth;
    }
    if (inImageHeight > 0) {
        ImageExtent.width = inImageHeight;
    }
    auto CopySize     = ImageExtent.width * ImageExtent.height * 4; // TODO don't hardcode this
    auto srcImageProp = inImage.GetImageProperty();
    const vk::CommandBuffer &Cmd = inCmdBuffer.GetCommandBufferHandle();

    Cmd.begin(vk::CommandBufferBeginInfo());
    auto ImageSubResource = vk::ImageSubresourceLayers(
         srcImageProp.mImageAspect, inMipLevel, inLayer, inLayersToBeCopied);
    auto BufferImageCopy = vk::BufferImageCopy(
         0, 0, 0, ImageSubResource, vk::Offset3D{0}, vk::Extent3D(ImageExtent, 1));
    inImage.CmdTransitionImageLayout(inCmdBuffer,
                                     inLayout,
                                     vk::ImageLayout::eTransferSrcOptimal,
                                     vk::PipelineStageFlagBits::eComputeShader,
                                     vk::PipelineStageFlagBits::eTransfer,
                                     vk::AccessFlagBits::eMemoryRead,
                                     vk::AccessFlagBits::eMemoryRead);
    Cmd.copyImageToBuffer(inImage.GetImageHandle(),
                          vk::ImageLayout::eTransferSrcOptimal,
                          mBufferHandle,
                          BufferImageCopy);
    inImage.CmdTransitionImageLayout(inCmdBuffer,
                                     vk::ImageLayout::eTransferSrcOptimal,
                                     inLayout,
                                     vk::PipelineStageFlagBits::eTransfer,
                                     vk::PipelineStageFlagBits::eFragmentShader,
                                     vk::AccessFlagBits::eMemoryRead,
                                     vk::AccessFlagBits::eMemoryRead);
    Cmd.end();
    inQueue.Submit<SubmitContext::SingleTime>(inCmdBuffer);
}

void VulkanBufferBase::FillBufferUsage(vk::BufferCreateInfo &inInfo,
                                       BufferContext inBufferContext,
                                       MemoryType inBufferStorageType) {
    /// @warning This effectively makes the BufferContext flags meaningless.
    /// This is done since, for now, the usage type for a buffer / image doesn't mean anything
    /// for performance. If later, if there is any specific hardware feature that might arrive,
    /// this might be important for then. But for now, @todo for later.
    inInfo.setUsage(vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer |
                    vk::BufferUsageFlagBits::eStorageBuffer |
                    vk::BufferUsageFlagBits::eUniformBuffer |
                    vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst);
}

void VulkanBuffer::MapMemoryRegion(void **outMappedMemoryRegion) {
    if (not mIsMemoryMapped) {
        mMappedMemoryRegion =
             static_cast<void *>(mDevice->mapMemory(mBufferMemory, 0, GetBufferSize()));
    }
    *outMappedMemoryRegion = mMappedMemoryRegion;
    mIsMemoryMapped        = true;
}
void VulkanBuffer::UnMapMemoryRegion() {
    if (mIsMemoryMapped) {
        mDevice->unmapMemory(mBufferMemory);
        mIsMemoryMapped = false;
    }
}

VulkanBuffer::VulkanBuffer(const VulkanDevice &inDevice,
                           size_t inSize,
                           BufferContext inBufferContext,
                           MemoryType inBufferMemoryType) {
    Init({&inDevice, inSize, inBufferContext, inBufferMemoryType});
}

VulkanBuffer::~VulkanBuffer() {
    if (mInitialized) {
        Destroy();
    }
}

void VulkanBuffer::Init(CreateInfo inCreateInfo) {
    VulkanBufferBase::Init({inCreateInfo.inDevice});
    mSize      = inCreateInfo.inSize;
    auto Usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;

    if (inCreateInfo.inBufferContext == BufferContext::Staging)
        Usage = vk::BufferUsageFlagBits::eTransferSrc;

    auto BufferCreateInfo =
         vk::BufferCreateInfo(vk::BufferCreateFlags(), inCreateInfo.inSize, Usage);
    FillBufferUsage(
         BufferCreateInfo, inCreateInfo.inBufferContext, inCreateInfo.inBufferMemoryType);

    mBufferHandle = mDevice->createBuffer(BufferCreateInfo);

    uint32_t MemoryTypeIndex;
    vk::DeviceSize MemorySize;
    vk::MemoryPropertyFlags MemoryPropertyFlagBits;

    if (inCreateInfo.inBufferMemoryType == MemoryType::DeviceLocal)
        MemoryPropertyFlagBits = vk::MemoryPropertyFlagBits::eDeviceLocal;
    else if (inCreateInfo.inBufferMemoryType == MemoryType::HostVisibleAndCoherenet)
        MemoryPropertyFlagBits =
             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    GetMemoryTypeIndex(MemoryPropertyFlagBits, mBufferHandle, MemorySize, MemoryTypeIndex);
    mBufferMemory = mDevice->allocateMemory(vk::MemoryAllocateInfo(MemorySize, MemoryTypeIndex));
    mDevice->bindBufferMemory(mBufferHandle, mBufferMemory, 0);
    mInitialized = true;
}

void VulkanBuffer::Destroy() {
    if (mIsMemoryMapped) {
        mDevice->unmapMemory(mBufferMemory);
    }
    mVulkanDevice->Wait();
    mDevice->destroyBuffer(mBufferHandle);
    mDevice->freeMemory(mBufferMemory);
    mInitialized = false;
}