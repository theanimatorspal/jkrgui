#include "VulkanImage.hpp"
#include "VulkanCommandBuffer.hpp"
#include "vulkan/vulkan_enums.hpp"
#include <VulkanBufferVMA.hpp>
#include <iostream>

using namespace ksai;

ksai::VulkanImageBase::VulkanImageBase(const VulkanDevice& inDevice, bool inDestroyImageView)
    : mPhysicalDevice(inDevice.GetPhysicalDeviceHandle()),
      mDevice(inDevice.GetDeviceHandle()),
      mDestroyImageView(inDestroyImageView) {}

ksai::VulkanImageBase::~VulkanImageBase() {
    if (mImageView and mDestroyImageView) mDevice.destroyImageView(mImageView);
}

void ksai::VulkanImageBase::SubmitImmediateCmdCopyFromData(
     const VulkanQueue<QueueContext::Graphics>& inQueue,
     const VulkanCommandBuffer& inCmdBuffer,
     const VulkanDevice& inDevice,
     const VulkanFence& inFence,
     void** inData,
     vk::DeviceSize inSize) {
    VulkanBuffer<BufferContext::Staging, MemoryType::HostVisibleAndCoherenet> StagingBuffer(
         inDevice, inSize);
    void* MapRegion;
    StagingBuffer.MapMemoryRegion(&MapRegion);
    std::memcpy(MapRegion, *inData, inSize);
    const vk::CommandBuffer& Cmd = inCmdBuffer.GetCommandBufferHandle();
    inFence.Reset();
    inFence.Wait();
    inCmdBuffer.Begin();
    vk::ImageSubresourceLayers ImageSubResourceLayer(
         mImageProperties.mImageAspect, 0, 0, mImageProperties.mArrayLayers);
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
    Cmd.copyBufferToImage(
         StagingBuffer.GetBufferHandle(), mImage, vk::ImageLayout::eTransferDstOptimal, Copy);
    CmdTransitionImageLayout(inCmdBuffer,
                             vk::ImageLayout::eTransferDstOptimal,
                             vk::ImageLayout::eGeneral,
                             vk::PipelineStageFlagBits::eTransfer,
                             vk::PipelineStageFlagBits::eBottomOfPipe,
                             vk::AccessFlagBits::eTransferWrite,
                             vk::AccessFlagBits::eNone);
    inCmdBuffer.End();
    inQueue.Submit<SubmitContext::SingleTime>(inCmdBuffer, inFence);
    //	inQueue.Wait();
}

void VulkanImageBase::SubmitImmediateCmdCopyFromData(
     const VulkanQueue<QueueContext::Graphics>& inQueue,
     const VulkanCommandBuffer& inCmdBuffer,
     const VulkanDevice& inDevice,
     const VulkanFence& inFence,
     vk::DeviceSize inSize,
     std::span<void**> inLayerImageDatas) {
    vk::DeviceSize size = inLayerImageDatas.size() * inSize;
    VulkanBuffer<BufferContext::Staging, MemoryType::HostVisibleAndCoherenet> StagingBuffer(
         inDevice, size);
    void* MapRegion;
    StagingBuffer.MapMemoryRegion(&MapRegion);
    for (int i = 0; i < inLayerImageDatas.size(); i++) {
        std::memcpy((char*)MapRegion + i * inSize, *inLayerImageDatas[i], inSize);
    }
    const vk::CommandBuffer& Cmd = inCmdBuffer.GetCommandBufferHandle();
    inFence.Reset();
    inFence.Wait();
    inCmdBuffer.Begin();
    vk::ImageSubresourceLayers ImageSubResourceLayer(
         mImageProperties.mImageAspect, 0, 0, mImageProperties.mArrayLayers);
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
    Cmd.copyBufferToImage(
         StagingBuffer.GetBufferHandle(), mImage, vk::ImageLayout::eTransferDstOptimal, Copy);
    CmdTransitionImageLayout(inCmdBuffer,
                             vk::ImageLayout::eTransferDstOptimal,
                             vk::ImageLayout::eGeneral,
                             vk::PipelineStageFlagBits::eTransfer,
                             vk::PipelineStageFlagBits::eBottomOfPipe,
                             vk::AccessFlagBits::eTransferWrite,
                             vk::AccessFlagBits::eNone);
    inCmdBuffer.End();
    inQueue.Submit<SubmitContext::SingleTime>(inCmdBuffer, inFence);
}

void ksai::VulkanImageBase::CmdTransitionImageLayout(const VulkanCommandBuffer& inBuffer,
                                                     vk::ImageLayout inOldImageLayout,
                                                     vk::ImageLayout inNewImageLayout,
                                                     vk::PipelineStageFlags inBeforeStage,
                                                     vk::PipelineStageFlags inAfterStage,
                                                     vk::AccessFlags inBeforeAccess,
                                                     vk::AccessFlags inAfterAccess) const {
    vk::ImageSubresourceRange ImageSubResourceRange(mImageProperties.mImageAspect,
                                                    0,
                                                    mImageProperties.mMipLevels,
                                                    0,
                                                    mImageProperties.mArrayLayers);
    auto ImgMemBarr = vk::ImageMemoryBarrier(inBeforeAccess,
                                             inAfterAccess,
                                             inOldImageLayout,
                                             inNewImageLayout,
                                             VK_QUEUE_FAMILY_IGNORED,
                                             VK_QUEUE_FAMILY_IGNORED,
                                             mImage,
                                             ImageSubResourceRange);
    auto& CmdBuf    = inBuffer.GetCommandBufferHandle();
    CmdBuf.pipelineBarrier(
         inBeforeStage, inAfterStage, vk::DependencyFlagBits::eByRegion, {}, {}, ImgMemBarr);
}

void ksai::VulkanImageBase::GetMemoryTypeIndex(vk::MemoryPropertyFlagBits inFlag,
                                               vk::Image inImage,
                                               vk::DeviceSize& outSize,
                                               ui& outIndex) {
    vk::PhysicalDeviceMemoryProperties memoryProperties = mPhysicalDevice.getMemoryProperties();
    vk::MemoryRequirements memoryRequirements = mDevice.getImageMemoryRequirements(inImage);
    ui typeBits                               = memoryRequirements.memoryTypeBits;
    ui typeIndex                              = ui(~0);
    for (ui i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if ((typeBits & 1) && ((memoryProperties.memoryTypes[i].propertyFlags &
                                vk::MemoryPropertyFlagBits::eDeviceLocal) == inFlag)) {
            typeIndex = i;
            outIndex  = typeIndex;
            outSize   = memoryRequirements.size;
            return;
        }
        typeBits >>= 1;
    }
}

void ksai::VulkanImageBase::GetImageTiling(vk::Format inFormat,
                                           vk::FormatFeatureFlagBits inFormatFeature,
                                           vk::ImageTiling& outTiling) {
    vk::FormatProperties FormatProperties = mPhysicalDevice.getFormatProperties(inFormat);
    vk::ImageTiling Tiling;

    // if (FormatProperties.linearTilingFeatures &
    // vk::FormatFeatureFlagBits::eDepthStencilAttachment)
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

void ksai::VulkanImageBase::CreateImageAndBindMemory(vk::Image& inImage,
                                                     vk::DeviceMemory& inDeviceMemory) {
    vk::ImageTiling Tiling;
    GetImageTiling(mImageProperties.mImageFormat, mImageProperties.mImageFormatFeature, Tiling);
    auto ImageCreateInfo = vk::ImageCreateInfo(vk::ImageCreateFlags(),
                                               mImageProperties.mImageType,
                                               mImageProperties.mImageFormat,
                                               vk::Extent3D(mImageProperties.mExtent, 1),
                                               mImageProperties.mMipLevels,
                                               mImageProperties.mArrayLayers,
                                               mImageProperties.mSampleCountFlagBits,
                                               mImageProperties.mTiling,
                                               mImageProperties.mImageUsage);
    inImage              = mDevice.createImage(ImageCreateInfo);
    vk::DeviceSize RequiredSize;
    ui RequiredIndex;
    GetMemoryTypeIndex(mImageProperties.mMemoryProperty, inImage, RequiredSize, RequiredIndex);
    inDeviceMemory = mDevice.allocateMemory(vk::MemoryAllocateInfo(RequiredSize, RequiredIndex));
    mDevice.bindImageMemory(inImage, inDeviceMemory, 0);
}

void ksai::VulkanImageBase::CreateImageView(vk::Image& inImage) {
    vk::ImageSubresourceRange ImageSubResourcesRange(mImageProperties.mImageAspect,
                                                     0,
                                                     mImageProperties.mMipLevels,
                                                     0,
                                                     mImageProperties.mArrayLayers);
    vk::ImageViewCreateInfo ImageViewCreateInfo(vk::ImageViewCreateFlags(),
                                                inImage,
                                                mImageProperties.mImageViewType,
                                                mImageProperties.mImageFormat,
                                                {},
                                                ImageSubResourcesRange);

    mImageView = mDevice.createImageView(ImageViewCreateInfo);
}

void ksai::VulkanImageBase::CmdCopyImageFromImageAfterStage(
     const VulkanQueue<QueueContext::Graphics>& inQueue,
     const VulkanCommandBuffer& inCmdBuffer,
     const VulkanDevice& inDevice,
     VulkanImageBase& inImage,
     vk::PipelineStageFlags inAfterStage,
     vk::AccessFlags inAfterStageAccessflags) {
    // TODO This has to be checked
    auto& SrcVulkanImage = inImage;
    auto& DstVulkanImage = *this;
    auto& SrcImageHandle = SrcVulkanImage.GetImageHandle();
    auto& DstImageHandle = SrcVulkanImage.GetImageHandle();
    auto SrcImageLayout  = SrcVulkanImage.GetInitialImageLayout();
    auto DstImageLayout  = DstVulkanImage.GetInitialImageLayout();
    auto SrcImageProp    = SrcVulkanImage.mImageProperties;
    auto DstImageProp    = DstVulkanImage.mImageProperties;
    auto& Cmd            = inCmdBuffer.GetCommandBufferHandle();
    vk::ImageSubresourceLayers SrcSubResource(
         SrcImageProp.mImageAspect, 0, 0, SrcImageProp.mArrayLayers);
    vk::ImageSubresourceLayers DstSubResource(
         DstImageProp.mImageAspect, 0, 0, DstImageProp.mArrayLayers);
    vk::ImageSubresourceRange SrcSubResourceRange(
         SrcSubResource.aspectMask, 0, 0, 0, SrcImageProp.mArrayLayers);
    vk::ImageSubresourceRange DstSubResourceRange(
         DstSubResource.aspectMask, 0, 0, 0, DstImageProp.mArrayLayers);
    vk::ImageCopy CopyRegion(SrcSubResource,
                             vk::Offset3D{},
                             DstSubResource,
                             vk::Offset3D{},
                             vk::Extent3D(SrcVulkanImage.GetImageExtent(), 0));

    DstVulkanImage.CmdTransitionImageLayout(inCmdBuffer,
                                            DstImageLayout,
                                            vk::ImageLayout::eTransferDstOptimal,
                                            inAfterStage,
                                            vk::PipelineStageFlagBits::eTransfer,
                                            inAfterStageAccessflags,
                                            vk::AccessFlagBits::eMemoryWrite);

    SrcVulkanImage.CmdTransitionImageLayout(inCmdBuffer,
                                            SrcImageLayout,
                                            vk::ImageLayout::eTransferSrcOptimal,
                                            inAfterStage,
                                            vk::PipelineStageFlagBits::eTransfer,
                                            inAfterStageAccessflags,
                                            vk::AccessFlagBits::eMemoryRead);

    Cmd.copyImage(SrcImageHandle, SrcImageLayout, DstImageHandle, DstImageLayout, CopyRegion);

    DstVulkanImage.CmdTransitionImageLayout(inCmdBuffer,
                                            vk::ImageLayout::eTransferDstOptimal,
                                            DstImageLayout,
                                            vk::PipelineStageFlagBits::eTransfer,
                                            vk::PipelineStageFlagBits::eFragmentShader,
                                            vk::AccessFlagBits::eMemoryWrite,
                                            vk::AccessFlagBits::eMemoryRead);

    SrcVulkanImage.CmdTransitionImageLayout(inCmdBuffer,
                                            vk::ImageLayout::eTransferSrcOptimal,
                                            SrcImageLayout,
                                            vk::PipelineStageFlagBits::eTransfer,
                                            vk::PipelineStageFlagBits::eFragmentShader,
                                            vk::AccessFlagBits::eMemoryRead,
                                            vk::AccessFlagBits::eMemoryRead);
}

void VulkanImageBase::FillImageProperties(ImageContext inImageContext, uint32_t inNumSamples) {
    // These are all for other than Defaults
    if (inImageContext == ImageContext::DepthImage) {
        mImageProperties.mImageFormat         = vk::Format::eD16Unorm;
        mImageProperties.mImageFormatFeature  = vk::FormatFeatureFlagBits::eDepthStencilAttachment;
        mImageProperties.mImageType           = vk::ImageType::e2D;
        mImageProperties.mSampleCountFlagBits = vk::SampleCountFlagBits::e1;
        mImageProperties.mImageUsage =
             vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
        mImageProperties.mImageAspect    = vk::ImageAspectFlagBits::eDepth;
        mImageProperties.mMemoryProperty = vk::MemoryPropertyFlagBits::eDeviceLocal;
        mImageProperties.mImageViewType  = vk::ImageViewType::e2D;
        // mImageProperties.mInitialImageLayout = vk::ImageLayout::eDepthAttachmentOptimal;
        mImageProperties.mInitialImageLayout = vk::ImageLayout::eGeneral;
    } else if (inImageContext == ImageContext::Storage) {
        mImageProperties.mImageUsage =
             vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc;
        mImageProperties.mImageFormat = vk::Format::eB8G8R8A8Unorm;
    } else if (inImageContext == ImageContext::Default) {
        mImageProperties.mImageUsage =
             vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
        mImageProperties.mImageFormat = vk::Format::eB8G8R8A8Unorm;
    } else if (inImageContext == ImageContext::CubeCompatible) {
        mImageProperties.mFlags = vk::ImageCreateFlagBits::eCubeCompatible;
        mImageProperties.mImageUsage =
             vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
        mImageProperties.mImageViewType = vk::ImageViewType::eCube;
        mImageProperties.mImageFormat   = vk::Format::eB8G8R8A8Unorm;
    } else if (inImageContext == ImageContext::ColorAttach) {
        mImageProperties.mImageUsage = vk::ImageUsageFlagBits::eColorAttachment |
                                       vk::ImageUsageFlagBits::eTransientAttachment;
        mImageProperties.mImageFormat = vk::Format::eB8G8R8A8Unorm; // This in on the surface, so
    }

    switch (inNumSamples) {
        case 1:
            mImageProperties.mSampleCountFlagBits = vk::SampleCountFlagBits::e1;
            break;
        case 2:
            mImageProperties.mSampleCountFlagBits = vk::SampleCountFlagBits::e2;
            break;
        case 4:
            mImageProperties.mSampleCountFlagBits = vk::SampleCountFlagBits::e4;
            break;
    }
}