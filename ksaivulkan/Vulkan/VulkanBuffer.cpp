#include "VulkanBuffer.hpp"

using namespace ksai;

ksai::VulkanBufferBase::VulkanBufferBase(const VulkanDevice& inDevice)
    : mVulkanDevice(inDevice),
      mDevice(inDevice.GetDeviceHandle()),
      mPhysicalDevice(inDevice.GetPhysicalDeviceHandle()) {}

void ksai::VulkanBufferBase::SubmitImmediateCmdCopyFrom(
     const VulkanQueue<QueueContext::Graphics>& inQueue,
     const VulkanCommandBuffer& inCmdBuffer,
     void* inData) {
    uint32_t MemoryIndex;
    vk::DeviceSize MemorySize;
    GetMemoryTypeIndex(vk::MemoryPropertyFlagBits::eHostVisible |
                            vk::MemoryPropertyFlagBits::eHostCoherent,
                       mBufferHandle,
                       MemorySize,
                       MemoryIndex);
    auto StagingBufferCreateInfo = vk::BufferCreateInfo(
         vk::BufferCreateFlags(), MemorySize, vk::BufferUsageFlagBits::eTransferSrc);
    auto StagingBuffer = mDevice.createBuffer(StagingBufferCreateInfo);
    auto StagingBufferMemory =
         mDevice.allocateMemory(vk::MemoryAllocateInfo(MemorySize, MemoryIndex));
    mDevice.bindBufferMemory(StagingBuffer, StagingBufferMemory, 0);
    uint8_t* pData = static_cast<uint8_t*>(mDevice.mapMemory(StagingBufferMemory, 0, MemorySize));
    std::memcpy(pData, inData, MemorySize);
    mDevice.unmapMemory(StagingBufferMemory);
    const vk::CommandBuffer& Cmd = inCmdBuffer.GetCommandBufferHandle();
    Cmd.begin(vk::CommandBufferBeginInfo());
    std::array<vk::BufferCopy, 1> Regions = {vk::BufferCopy(0, 0, MemorySize)};
    inCmdBuffer.GetCommandBufferHandle().copyBuffer(StagingBuffer, mBufferHandle, Regions);
    Cmd.end();
    inQueue.Submit<SubmitContext::SingleTime>(inCmdBuffer);
    mDevice.freeMemory(StagingBufferMemory);
    mDevice.destroyBuffer(StagingBuffer);
}

void ksai::VulkanBufferBase::CmdCopyFrom(const VulkanCommandBuffer& inCmdBuffer,
                                         VulkanBufferBase& inBuffer,
                                         vk::DeviceSize FromBufferOffset,
                                         vk::DeviceSize ToBufferOffset,
                                         vk::DeviceSize inSizeToCopy) {
    vk::BufferCopy BufferCopyRegion =
         vk::BufferCopy(FromBufferOffset, ToBufferOffset, inSizeToCopy);
    inCmdBuffer.GetCommandBufferHandle().copyBuffer(
         inBuffer.mBufferHandle, mBufferHandle, BufferCopyRegion);
}

void ksai::VulkanBufferBase::SetBarrier(const VulkanCommandBuffer& inCmdBuffer,
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
                                                vk::DeviceSize& outSize,
                                                uint32_t& outIndex) {
    vk::PhysicalDeviceMemoryProperties memoryProperties = mPhysicalDevice.getMemoryProperties();
    vk::MemoryRequirements memoryRequirements = mDevice.getBufferMemoryRequirements(inBuffer);
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

void ksai::VulkanBufferBase::CmdCopyFromImage(const VulkanCommandBuffer& inCmdBuffer,
                                              const VulkanImageBase& inImage,
                                              vk::PipelineStageFlags inAfterStage,
                                              vk::AccessFlags inAfterAccessFlags) const {
    // TODO
}

void ksai::VulkanBufferBase::SubmitImmediateCmdCopyFromImage(
     const VulkanQueue<QueueContext::Graphics>& inQueue,
     const VulkanCommandBuffer& inCmdBuffer,
     VulkanImageBase& inImage) const {
    auto ImageExtent  = inImage.GetImageExtent();
    auto CopySize     = ImageExtent.width * ImageExtent.height * 4; // TODO don't hardcode this
    auto srcImageProp = inImage.GetImageProperty();
    const vk::CommandBuffer& Cmd = inCmdBuffer.GetCommandBufferHandle();

    Cmd.begin(vk::CommandBufferBeginInfo());
    auto ImageSubResource =
         vk::ImageSubresourceLayers(srcImageProp.mImageAspect, 0, 0, srcImageProp.mArrayLayers);
    auto BufferImageCopy = vk::BufferImageCopy(
         0, 0, 0, ImageSubResource, vk::Offset3D{0}, vk::Extent3D(ImageExtent, 1));
    inImage.CmdTransitionImageLayout(inCmdBuffer,
                                     vk::ImageLayout::eGeneral,
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
                                     vk::ImageLayout::eGeneral,
                                     vk::PipelineStageFlagBits::eTransfer,
                                     vk::PipelineStageFlagBits::eFragmentShader,
                                     vk::AccessFlagBits::eMemoryRead,
                                     vk::AccessFlagBits::eMemoryRead);
    Cmd.end();
    inQueue.Submit<SubmitContext::SingleTime>(inCmdBuffer);
}

void VulkanBufferBase::FillBufferUsage(vk::BufferCreateInfo& inInfo,
                                       BufferContext inBufferContext,
                                       MemoryType inBufferStorageType) {
    // if (inBufferContext == BufferContext::Vertex)
    //     inInfo.setUsage(vk::BufferUsageFlagBits::eVertexBuffer |
    //                     vk::BufferUsageFlagBits::eTransferDst);
    // else if (inBufferContext == BufferContext::Index)
    //     inInfo.setUsage(vk::BufferUsageFlagBits::eIndexBuffer |
    //                     vk::BufferUsageFlagBits::eTransferDst);
    // else if (inBufferContext == BufferContext::Uniform)
    //     inInfo.setUsage(vk::BufferUsageFlagBits::eUniformBuffer |
    //                     vk::BufferUsageFlagBits::eTransferDst);
    // else if (inBufferContext == BufferContext::Storage)
    //     inInfo.setUsage(vk::BufferUsageFlagBits::eStorageBuffer |
    //                     vk::BufferUsageFlagBits::eTransferDst);
    // else if (inBufferContext == (BufferContext::Vertex | BufferContext::Storage))
    //     inInfo.setUsage(vk::BufferUsageFlagBits::eStorageBuffer |
    //                     vk::BufferUsageFlagBits::eVertexBuffer |
    //                     vk::BufferUsageFlagBits::eTransferDst);
    // else if (inBufferContext == BufferContext::Staging)
    //     inInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc);
    // else if (inBufferContext == (BufferContext::Uniform | BufferContext::Storage))
    //     inInfo.setUsage(vk::BufferUsageFlagBits::eStorageBuffer |
    //                     vk::BufferUsageFlagBits::eUniformBuffer |
    //                     vk::BufferUsageFlagBits::eTransferDst);

    // if (inBufferContext == (BufferContext::Staging | BufferContext::Index)) {
    //     inInfo.setUsage(vk::BufferUsageFlagBits::eIndexBuffer |
    //                     vk::BufferUsageFlagBits::eStorageBuffer);
    // }
    inInfo.setUsage(vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer |
                    vk::BufferUsageFlagBits::eStorageBuffer |
                    vk::BufferUsageFlagBits::eUniformBuffer |
                    vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst);
}

void VulkanBuffer::MapMemoryRegion(void** outMappedMemoryRegion) {
    *outMappedMemoryRegion =
         static_cast<void*>(mDevice.mapMemory(mBufferMemory, 0, GetBufferSize()));
    mIsMemoryMapped = true;
}
void VulkanBuffer::UnMapMemoryRegion() {
    mDevice.unmapMemory(mBufferMemory);

    mIsMemoryMapped = false;
}

VulkanBuffer::VulkanBuffer(const VulkanDevice& inDevice,
                           size_t inSize,
                           BufferContext inBufferContext,
                           MemoryType inBufferMemoryType)
    : VulkanBufferBase(inDevice) {
    mSize      = inSize;
    auto Usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;

    if (inBufferContext == BufferContext::Staging) Usage = vk::BufferUsageFlagBits::eTransferSrc;

    auto BufferCreateInfo = vk::BufferCreateInfo(vk::BufferCreateFlags(), inSize, Usage);
    FillBufferUsage(BufferCreateInfo, inBufferContext, inBufferMemoryType);

    mBufferHandle = mDevice.createBuffer(BufferCreateInfo);

    uint32_t MemoryTypeIndex;
    vk::DeviceSize MemorySize;
    vk::MemoryPropertyFlags MemoryPropertyFlagBits;

    if (inBufferMemoryType == MemoryType::DeviceLocal)
        MemoryPropertyFlagBits = vk::MemoryPropertyFlagBits::eDeviceLocal;
    else if (inBufferMemoryType == MemoryType::HostVisibleAndCoherenet)
        MemoryPropertyFlagBits =
             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

    GetMemoryTypeIndex(MemoryPropertyFlagBits, mBufferHandle, MemorySize, MemoryTypeIndex);
    mBufferMemory = mDevice.allocateMemory(vk::MemoryAllocateInfo(MemorySize, MemoryTypeIndex));
    mDevice.bindBufferMemory(mBufferHandle, mBufferMemory, 0);
}

VulkanBuffer::~VulkanBuffer() {
    if (mIsMemoryMapped) {
        mDevice.unmapMemory(mBufferMemory);
    }
    mVulkanDevice.Wait();
    mDevice.destroyBuffer(mBufferHandle);
    mDevice.freeMemory(mBufferMemory);
}