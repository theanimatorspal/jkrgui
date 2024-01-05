#include "VulkanImage.hpp"
#include "VulkanCommandBuffer.hpp"
#include <VulkanBufferVMA.hpp>
#include <iostream>

using namespace ksai;

ksai::VulkanImageBase::VulkanImageBase(const VulkanDevice& inDevice)
    : mPhysicalDevice(inDevice.GetPhysicalDeviceHandle())
    , mDevice(inDevice.GetDeviceHandle())
{
}

ksai::VulkanImageBase::~VulkanImageBase()
{
    if (mImageView)
        mDevice.destroyImageView(mImageView);
}

void ksai::VulkanImageBase::SubmitImmediateCmdCopyFromData(const VulkanQueue<QueueContext::Graphics>& inQueue, const VulkanCommandBuffer& inCmdBuffer, const VulkanDevice& inDevice, void** inData, vk::DeviceSize inSize)
{
    VulkanBuffer<BufferContext::Staging, MemoryType::HostVisibleAndCoherenet> StagingBuffer(inDevice, inSize);
    void* MapRegion;
    StagingBuffer.MapMemoryRegion(&MapRegion);
    std::memcpy(MapRegion, *inData, inSize);
    const vk::CommandBuffer& Cmd = inCmdBuffer.GetCommandBufferHandle();
    Cmd.begin(vk::CommandBufferBeginInfo());
    vk::ImageSubresourceLayers ImageSubResourceLayer(mImageProperties.mImageAspect, 0, 0, mImageProperties.mArrayLayers);
    vk::BufferImageCopy Copy(0, mImageProperties.mExtent.width, mImageProperties.mExtent.height, ImageSubResourceLayer, vk::Offset3D(0, 0, 0), vk::Extent3D(mImageProperties.mExtent, 1));
    CmdTransitionImageLayout(inCmdBuffer, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, vk::AccessFlagBits::eNone, vk::AccessFlagBits::eTransferWrite);
    Cmd.copyBufferToImage(StagingBuffer.GetBufferHandle(), mImage, vk::ImageLayout::eTransferDstOptimal, Copy);
    CmdTransitionImageLayout(inCmdBuffer, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eGeneral, vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe, vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eNone);
    Cmd.end();
    inQueue.Submit<SubmitContext::SingleTime>(inCmdBuffer);
    //	inQueue.Wait();
}

void VulkanImageBase::SubmitImmediateCmdCopyFromData(
    const VulkanQueue<QueueContext::Graphics> &inQueue,
    const VulkanCommandBuffer &inCmdBuffer,
    const VulkanDevice &inDevice,
    vk::DeviceSize inSize,
    std::span<void **> inLayerImageDatas)
{
    vk::DeviceSize size = inLayerImageDatas.size() * inSize;
    VulkanBuffer<BufferContext::Staging, MemoryType::HostVisibleAndCoherenet> StagingBuffer(inDevice,
                                                                                            size);
    void *MapRegion;
    StagingBuffer.MapMemoryRegion(&MapRegion);
    for (int i = 0; i < size; i++) {
        std::memcpy((char *) MapRegion + i * inSize, *inLayerImageDatas[i], inSize);
    }
    const vk::CommandBuffer &Cmd = inCmdBuffer.GetCommandBufferHandle();
    Cmd.begin(vk::CommandBufferBeginInfo());
    vk::ImageSubresourceLayers ImageSubResourceLayer(mImageProperties.mImageAspect,
                                                     0,
                                                     0,
                                                     mImageProperties.mArrayLayers);
    vk::BufferImageCopy Copy(0,
                             mImageProperties.mExtent.width,
                             mImageProperties.mExtent.height,
                             ImageSubResourceLayer,
                             vk::Offset3D(0, 0, 0),
                             vk::Extent3D(mImageProperties.mExtent, 1));
    CmdTransitionImageLayout(inCmdBuffer,
                             vk::ImageLayout::eUndefined,
                             vk::ImageLayout::eTransferDstOptimal,
                             vk::PipelineStageFlagBits::eTopOfPipe,
                             vk::PipelineStageFlagBits::eTransfer,
                             vk::AccessFlagBits::eNone,
                             vk::AccessFlagBits::eTransferWrite);
    Cmd.copyBufferToImage(StagingBuffer.GetBufferHandle(),
                          mImage,
                          vk::ImageLayout::eTransferDstOptimal,
                          Copy);
    CmdTransitionImageLayout(inCmdBuffer,
                             vk::ImageLayout::eTransferDstOptimal,
                             vk::ImageLayout::eGeneral,
                             vk::PipelineStageFlagBits::eTransfer,
                             vk::PipelineStageFlagBits::eBottomOfPipe,
                             vk::AccessFlagBits::eTransferWrite,
                             vk::AccessFlagBits::eNone);
    Cmd.end();
    inQueue.Submit<SubmitContext::SingleTime>(inCmdBuffer);
}

void ksai::VulkanImageBase::CmdTransitionImageLayout(const VulkanCommandBuffer& inBuffer, vk::ImageLayout inOldImageLayout, vk::ImageLayout inNewImageLayout, vk::PipelineStageFlags inBeforeStage, vk::PipelineStageFlags inAfterStage, vk::AccessFlags inBeforeAccess, vk::AccessFlags inAfterAccess)
{
    vk::ImageSubresourceRange ImageSubResourceRange(mImageProperties.mImageAspect, 0, mImageProperties.mMipLevels, 0, mImageProperties.mArrayLayers);
    auto ImgMemBarr = vk::ImageMemoryBarrier(inBeforeAccess,
                                             inAfterAccess,
                                             inOldImageLayout,
                                             inNewImageLayout,
                                             VK_QUEUE_FAMILY_IGNORED,
                                             VK_QUEUE_FAMILY_IGNORED,
                                             mImage,
                                             ImageSubResourceRange);
    auto& CmdBuf = inBuffer.GetCommandBufferHandle();
    CmdBuf.pipelineBarrier(
        inBeforeStage, inAfterStage,
        vk::DependencyFlagBits::eByRegion,
        {}, {}, ImgMemBarr);
}

void ksai::VulkanImageBase::GetMemoryTypeIndex(vk::MemoryPropertyFlagBits inFlag, vk::Image inImage, vk::DeviceSize& outSize, ui& outIndex)
{
    vk::PhysicalDeviceMemoryProperties memoryProperties = mPhysicalDevice.getMemoryProperties();
    vk::MemoryRequirements memoryRequirements = mDevice.getImageMemoryRequirements(inImage);
    ui typeBits = memoryRequirements.memoryTypeBits;
    ui typeIndex = ui(~0);
    for (ui i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if ((typeBits & 1) && ((memoryProperties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) == inFlag)) {
            typeIndex = i;
            outIndex = typeIndex;
            outSize = memoryRequirements.size;
            return;
        }
        typeBits >>= 1;
    }
}

void ksai::VulkanImageBase::GetImageTiling(vk::Format inFormat, vk::FormatFeatureFlagBits inFormatFeature, vk::ImageTiling& outTiling)
{
    vk::FormatProperties FormatProperties = mPhysicalDevice.getFormatProperties(inFormat);
    vk::ImageTiling Tiling;

    // if (FormatProperties.linearTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
    if (FormatProperties.linearTilingFeatures & inFormatFeature) {
        Tiling = vk::ImageTiling::eLinear;
    } else if (FormatProperties.optimalTilingFeatures & inFormatFeature) {
        Tiling = vk::ImageTiling::eOptimal;
    } else {
        Tiling = vk::ImageTiling::eOptimal;
        std::cout << "Error Image Tiling\n";
        return;
    }

    outTiling = Tiling;
}

void ksai::VulkanImageBase::CreateImageAndBindMemory(vk::Image& inImage, vk::DeviceMemory& inDeviceMemory)
{
    vk::ImageTiling Tiling;
    GetImageTiling(mImageProperties.mImageFormat, mImageProperties.mImageFormatFeature, Tiling);
    auto ImageCreateInfo = vk::ImageCreateInfo(
        vk::ImageCreateFlags(),
        mImageProperties.mImageType,
        mImageProperties.mImageFormat,
        vk::Extent3D(mImageProperties.mExtent, 1),
        mImageProperties.mMipLevels,
        mImageProperties.mArrayLayers,
        mImageProperties.mSampleCountFlagBits,
        mImageProperties.mTiling,
        mImageProperties.mImageUsage);
    inImage = mDevice.createImage(ImageCreateInfo);
    vk::DeviceSize RequiredSize;
    ui RequiredIndex;
    GetMemoryTypeIndex(mImageProperties.mMemoryProperty, inImage, RequiredSize, RequiredIndex);
    inDeviceMemory = mDevice.allocateMemory(vk::MemoryAllocateInfo(RequiredSize, RequiredIndex));
    mDevice.bindImageMemory(inImage, inDeviceMemory, 0);
}

void ksai::VulkanImageBase::CreateImageView(vk::Image& inImage)
{
    vk::ImageSubresourceRange ImageSubResourcesRange(mImageProperties.mImageAspect, 0, mImageProperties.mMipLevels, 0, mImageProperties.mArrayLayers);
    vk::ImageViewCreateInfo ImageViewCreateInfo(
        vk::ImageViewCreateFlags(),
        inImage,
        mImageProperties.mImageViewType,
        mImageProperties.mImageFormat,
        {},
        ImageSubResourcesRange);

    mImageView = mDevice.createImageView(ImageViewCreateInfo);
}
