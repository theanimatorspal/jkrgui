#include "VulkanImage.hpp"
#include "VulkanCommandBuffer.hpp"
#include "vulkan/vulkan_enums.hpp"
#include <VulkanBufferVMA.hpp>
#include <iostream>
#include <Vendor/stbi/stb_image_write.h>
using namespace ksai;

ksai::VulkanImageBase::VulkanImageBase(VulkanDevice &inDevice, bool inDestroyImageView) {
    Init({&inDevice, inDestroyImageView});
}

void ksai::VulkanImageBase::Init(ksai::VulkanImageBase::CreateInfo inInfo) {
    mPhysicalDevice   = &inInfo.mDevice->GetPhysicalDeviceHandle();
    mDevice           = &inInfo.mDevice->GetDeviceHandle();
    mDestroyImageView = inInfo.mDestroyImageView;
    mInitialized      = true;
}

ksai::VulkanImageBase::~VulkanImageBase() {
    if (mInitialized) {
        Destroy();
    }
}

void ksai::VulkanImageBase::Destroy() {
    if (mImageView and mDestroyImageView) mDevice->destroyImageView(mImageView);
    for (auto imageView : mImageViews) {
        mDevice->destroyImageView(imageView);
    }
    mInitialized = false;
}
void ksai::VulkanImageBase::SubmitImmediateCmdCopyFromDataWithStagingBuffer(
     VulkanQueue<QueueContext::Graphics> &inQueue,
     VulkanCommandBuffer &inCmdBuffer,
     VulkanDevice &inDevice,
     VulkanFence &inFence,
     void **inData,
     vk::DeviceSize inSize,
     VulkanBufferVMA &inStagingBuffer,
     vk::ImageLayout inImageLayout,
     int inImageWidth,
     int inImageHeight,
     int inMipLevel,
     int inLayer,
     int inLayersToBeCopied) {

    void *MapRegion;
    inStagingBuffer.MapMemoryRegion(&MapRegion);
    std::memcpy(MapRegion, *inData, inSize);
    vk::CommandBuffer &Cmd = inCmdBuffer.GetCommandBufferHandle();
    inFence.Wait();
    inFence.Reset();
    inCmdBuffer.Begin();
    auto ImageProperties = mImageProperties;
    if (inImageWidth > 0) {
        ImageProperties.mExtent.width = inImageWidth;
    }
    if (inImageHeight > 0) {
        ImageProperties.mExtent.height = inImageHeight;
    }
    vk::ImageSubresourceLayers ImageSubResourceLayer(
         ImageProperties.mImageAspect, inMipLevel, inLayer, inLayersToBeCopied);
    vk::BufferImageCopy Copy(0,
                             ImageProperties.mExtent.width,
                             ImageProperties.mExtent.height,
                             ImageSubResourceLayer,
                             vk::Offset3D(0, 0, 0),
                             vk::Extent3D(ImageProperties.mExtent, 1));
    CmdTransitionImageLayout(inCmdBuffer,
                             vk::ImageLayout::eUndefined,
                             vk::ImageLayout::eTransferDstOptimal,
                             vk::PipelineStageFlagBits::eTopOfPipe,
                             vk::PipelineStageFlagBits::eTransfer,
                             vk::AccessFlagBits::eNone,
                             vk::AccessFlagBits::eTransferWrite);
    Cmd.copyBufferToImage(
         inStagingBuffer.GetBufferHandle(), mImage, vk::ImageLayout::eTransferDstOptimal, Copy);
    CmdTransitionImageLayout(inCmdBuffer,
                             vk::ImageLayout::eTransferDstOptimal,
                             inImageLayout,
                             vk::PipelineStageFlagBits::eTransfer,
                             vk::PipelineStageFlagBits::eBottomOfPipe,
                             vk::AccessFlagBits::eTransferWrite,
                             vk::AccessFlagBits::eNone);
    inCmdBuffer.End();
    inQueue.Submit<SubmitContext::SingleTime>(inCmdBuffer, inFence);
    // inStagingBuffer.UnMapMemoryRegion();
    // inQueue.Wait();
}

void VulkanImageBase::SubmitImmediateCmdCopyFromDataWithStagingBuffer(
     VulkanQueue<QueueContext::Graphics> &inQueue,
     VulkanCommandBuffer &inCmdBuffer,
     VulkanDevice &inDevice,
     VulkanFence &inFence,
     vk::DeviceSize inSize,
     std::span<void **> inLayerImageDatas,
     VulkanBufferVMA &StagingBuffer) {
    vk::DeviceSize size = inLayerImageDatas.size() * inSize;
    void *MapRegion;
    StagingBuffer.MapMemoryRegion(&MapRegion);
    for (int i = 0; i < inLayerImageDatas.size(); i++) {
        std::memcpy((char *)MapRegion + i * inSize, *inLayerImageDatas[i], inSize);
    }
    vk::CommandBuffer &Cmd = inCmdBuffer.GetCommandBufferHandle();
    inFence.Wait();
    inFence.Reset();
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

void ksai::VulkanImageBase::CmdTransitionImageLayout(VulkanCommandBuffer &inBuffer,
                                                     vk::ImageLayout inOldImageLayout,
                                                     vk::ImageLayout inNewImageLayout,
                                                     vk::PipelineStageFlags inBeforeStage,
                                                     vk::PipelineStageFlags inAfterStage,
                                                     vk::AccessFlags inBeforeAccess,
                                                     vk::AccessFlags inAfterAccess) {
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
    auto &CmdBuf    = inBuffer.GetCommandBufferHandle();
    CmdBuf.pipelineBarrier(
         inBeforeStage, inAfterStage, vk::DependencyFlagBits::eByRegion, {}, {}, ImgMemBarr);
    this->mImageProperties.mCurrentImageLayout = inNewImageLayout;
}

void ksai::VulkanImageBase::GetMemoryTypeIndex(vk::MemoryPropertyFlagBits inFlag,
                                               vk::Image inImage,
                                               vk::DeviceSize &outSize,
                                               ui &outIndex) {
    vk::PhysicalDeviceMemoryProperties memoryProperties = mPhysicalDevice->getMemoryProperties();
    vk::MemoryRequirements memoryRequirements = mDevice->getImageMemoryRequirements(inImage);
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
                                           vk::ImageTiling &outTiling) {
    vk::FormatProperties FormatProperties = mPhysicalDevice->getFormatProperties(inFormat);
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

void ksai::VulkanImageBase::CreateImageAndBindMemory(vk::Image &inImage,
                                                     vk::DeviceMemory &inDeviceMemory) {
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
    inImage              = mDevice->createImage(ImageCreateInfo);
    vk::DeviceSize RequiredSize;
    ui RequiredIndex;
    GetMemoryTypeIndex(mImageProperties.mMemoryProperty, inImage, RequiredSize, RequiredIndex);
    inDeviceMemory = mDevice->allocateMemory(vk::MemoryAllocateInfo(RequiredSize, RequiredIndex));
    mDevice->bindImageMemory(inImage, inDeviceMemory, 0);
}

void ksai::VulkanImageBase::CreateImageView(vk::Image &inImage) {
    vk::ImageSubresourceRange ImageSubResourcesRange(mImageProperties.mImageAspect,
                                                     0,
                                                     mImageProperties.mMipLevels,
                                                     0,
                                                     VK_REMAINING_ARRAY_LAYERS);
    vk::ImageViewCreateInfo ImageViewCreateInfo(vk::ImageViewCreateFlags(),
                                                inImage,
                                                mImageProperties.mImageViewType,
                                                mImageProperties.mImageFormat,
                                                {},
                                                ImageSubResourcesRange);

    mImageView = mDevice->createImageView(ImageViewCreateInfo);
}

void ksai::VulkanImageBase::CmdCopyImageFromImageAfterStage(
     VulkanCommandBuffer &inCmdBuffer,
     VulkanDevice &inDevice,
     VulkanImageBase &inImage,
     vk::PipelineStageFlags inAfterStage,
     vk::AccessFlags inAfterStageAccessflags,
     int inImageWidth,
     int inImageHeight,
     int inFromMipLevel,
     int inFromBaseArrayLayer,
     int inToMipLevel,
     int inToBaseArrayLayer,
     int inArrayLayersToBeCopied,
     vk::ImageLayout inSrcImageLayoutTobeSetTo,
     vk::ImageLayout inDstImageLayoutTobeSetTo,
     opt<vk::ImageLayout> inSrcImageLayoutFrom,
     opt<vk::ImageLayout> inDstImageLayoutFrom) {
    // TODO This has to be checked
    auto &SrcVulkanImage    = inImage;
    auto &DstVulkanImage    = *this;
    auto &SrcImageHandle    = SrcVulkanImage.GetImageHandle();
    auto &DstImageHandle    = DstVulkanImage.GetImageHandle();
    auto SrcImageLayoutFrom = inSrcImageLayoutFrom.has_value()
                                   ? inSrcImageLayoutFrom.value()
                                   : SrcVulkanImage.GetCurrentImageLayout();
    auto DstImageLayoutFrom = inDstImageLayoutFrom.has_value()
                                   ? inDstImageLayoutFrom.value()
                                   : DstVulkanImage.GetCurrentImageLayout();
    auto SrcImageProp       = SrcVulkanImage.mImageProperties;
    auto DstImageProp       = DstVulkanImage.mImageProperties;
    auto &Cmd               = inCmdBuffer.GetCommandBufferHandle();
    vk::ImageSubresourceLayers SrcSubResource(
         SrcImageProp.mImageAspect, inFromMipLevel, inFromBaseArrayLayer, inArrayLayersToBeCopied);
    vk::ImageSubresourceLayers DstSubResource(
         DstImageProp.mImageAspect, inToMipLevel, inToBaseArrayLayer, inArrayLayersToBeCopied);
    vk::ImageSubresourceRange SrcSubResourceRange(
         SrcSubResource.aspectMask, 0, 0, 0, inArrayLayersToBeCopied);
    vk::ImageSubresourceRange DstSubResourceRange(
         DstSubResource.aspectMask, 0, 0, 0, inArrayLayersToBeCopied);
    vk::ImageCopy CopyRegion(SrcSubResource,
                             vk::Offset3D{},
                             DstSubResource,
                             vk::Offset3D{},
                             vk::Extent3D(SrcVulkanImage.GetImageExtent(), 1));

    if (inImageWidth != -1) {
        CopyRegion.extent.width = inImageWidth;
    }
    if (inImageHeight != -1) {
        CopyRegion.extent.height = inImageHeight;
    }

    DstVulkanImage.CmdTransitionImageLayout(inCmdBuffer,
                                            DstImageLayoutFrom,
                                            vk::ImageLayout::eTransferDstOptimal,
                                            inAfterStage,
                                            vk::PipelineStageFlagBits::eTransfer,
                                            inAfterStageAccessflags,
                                            vk::AccessFlagBits::eMemoryWrite);

    SrcVulkanImage.CmdTransitionImageLayout(inCmdBuffer,
                                            SrcImageLayoutFrom,
                                            vk::ImageLayout::eTransferSrcOptimal,
                                            inAfterStage,
                                            vk::PipelineStageFlagBits::eTransfer,
                                            inAfterStageAccessflags,
                                            vk::AccessFlagBits::eMemoryRead);

    Cmd.copyImage(SrcImageHandle,
                  vk::ImageLayout::eTransferSrcOptimal,
                  DstImageHandle,
                  vk::ImageLayout::eTransferDstOptimal,
                  CopyRegion);

    DstVulkanImage.CmdTransitionImageLayout(inCmdBuffer,
                                            vk::ImageLayout::eTransferDstOptimal,
                                            inDstImageLayoutTobeSetTo,
                                            vk::PipelineStageFlagBits::eTransfer,
                                            vk::PipelineStageFlagBits::eFragmentShader,
                                            vk::AccessFlagBits::eMemoryWrite,
                                            vk::AccessFlagBits::eMemoryRead);

    SrcVulkanImage.CmdTransitionImageLayout(inCmdBuffer,
                                            vk::ImageLayout::eTransferSrcOptimal,
                                            inSrcImageLayoutTobeSetTo,
                                            vk::PipelineStageFlagBits::eTransfer,
                                            vk::PipelineStageFlagBits::eFragmentShader,
                                            vk::AccessFlagBits::eMemoryRead,
                                            vk::AccessFlagBits::eMemoryRead);
}

std::vector<char>
VulkanImageBase::SubmitImmediateCmdGetImageToVector(VulkanQueue<QueueContext::Graphics> &inQueue,
                                                    VulkanBufferVMA &inStagingBuffer,
                                                    VulkanCommandBuffer &inBuffer,
                                                    VulkanFence &inCommandBufferFence,
                                                    vk::ImageLayout inLayout,
                                                    int inImageWidth,
                                                    int inImageHeight,
                                                    int inMipLevel,
                                                    int inLayer,
                                                    int inLayersToBeCopied) {
    ///@todo @warning Hard Coded Image Channels
    ui ImageChannels = 4;
    auto ImageExtent = mImageProperties.mExtent;
    if (inImageWidth > 0) {
        ImageExtent.width = inImageWidth;
    }
    if (inImageHeight > 0) {
        ImageExtent.height = inImageHeight;
    }
    inStagingBuffer.SubmitImmediateCmdCopyFromImage(inQueue,
                                                    inBuffer,
                                                    inCommandBufferFence,
                                                    *this,
                                                    inLayout,
                                                    inMipLevel,
                                                    inLayer,
                                                    inLayersToBeCopied,
                                                    inImageWidth,
                                                    inImageHeight);
    void *MemoryRegion;
    inStagingBuffer.MapMemoryRegion(&MemoryRegion);
    std::vector<char> OutImage;
    uint32_t size = ImageChannels * ImageExtent.width * ImageExtent.height * inLayersToBeCopied;
    OutImage.resize(size);
    std::memcpy(OutImage.data(), MemoryRegion, size);
    // static int i = 0;
    // stbi_write_bmp(s("FILE DBG " + std::to_string(i++) + ".bmp").c_str(),
    //                ImageExtent.width,
    //                ImageExtent.height,
    //                4,
    //                OutImage.data());

    return OutImage;
}

void VulkanImageBase::FillImageProperties(ImageContext inImageContext, uint32_t inNumSamples) {
    if (mImageProperties.mArrayLayers > 1) {
        mImageProperties.mImageViewType = vk::ImageViewType::e2DArray;
    }
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
        // mImageProperties.mInitialImageLayout = vk::ImageLayout::eDepthAttachmentOptimal;
        mImageProperties.mInitialImageLayout = vk::ImageLayout::eGeneral;
    } else if (inImageContext == ImageContext::Storage) {
        mImageProperties.mImageUsage = vk::ImageUsageFlagBits::eStorage |
                                       vk::ImageUsageFlagBits::eTransferSrc |
                                       vk::ImageUsageFlagBits::eTransferDst;
        mImageProperties.mImageFormat = vk::Format::eR8G8B8A8Unorm;
    } else if (inImageContext == ImageContext::Default) {
        mImageProperties.mImageUsage = vk::ImageUsageFlagBits::eSampled |
                                       vk::ImageUsageFlagBits::eTransferDst |
                                       vk::ImageUsageFlagBits::eTransferSrc;
        mImageProperties.mImageFormat = vk::Format::eR8G8B8A8Unorm;
    } else if (inImageContext == ImageContext::CubeCompatible) {
        mImageProperties.mFlags = vk::ImageCreateFlagBits::eCubeCompatible;
        mImageProperties.mImageUsage =
             vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
        mImageProperties.mImageViewType = vk::ImageViewType::eCube;
        mImageProperties.mImageFormat   = vk::Format::eR8G8B8A8Unorm;
    } else if (inImageContext == ImageContext::ColorAttach) {
        mImageProperties.mImageUsage = vk::ImageUsageFlagBits::eColorAttachment |
                                       vk::ImageUsageFlagBits::eTransientAttachment;
        mImageProperties.mImageFormat = vk::Format::eR8G8B8A8Unorm; // This in on the surface, so
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

void VulkanImageBase::BuildImageViewsByLayers() {
    for (int i = 0; i < mImageProperties.mArrayLayers; i++) {
        vk::ImageSubresourceRange ImageSubResourcesRange(mImageProperties.mImageAspect,
                                                         0,
                                                         mImageProperties.mMipLevels,
                                                         i,
                                                         VK_REMAINING_ARRAY_LAYERS);
        vk::ImageViewCreateInfo ImageViewCreateInfo(vk::ImageViewCreateFlags(),
                                                    mImage,
                                                    mImageProperties.mImageViewType,
                                                    mImageProperties.mImageFormat,
                                                    {},
                                                    ImageSubResourcesRange);

        mImageViews.push_back(mDevice->createImageView(ImageViewCreateInfo));
    }
}

void VulkanImage::MoveMembers(ksai::VulkanImage &Other) {
    mImage                   = std::move(Other.mImage);
    mImageView               = std::move(Other.mImageView);
    mDeviceMemory            = std::move(Other.mDeviceMemory);
    mImageProperties.mExtent = std::move(Other.mImageProperties.mExtent);
    Other.mImage             = nullptr;
    Other.mImageView         = nullptr;
    Other.mDeviceMemory      = nullptr;
}

VulkanImage::VulkanImage(VulkanDevice &inDevice, ImageContext inImageContext) {
    CreateInfo info;
    info.mDevice       = &inDevice;
    info.mImageContext = inImageContext;
    Init(info);
}

VulkanImage::VulkanImage(VulkanDevice &inDevice,
                         ui inWidth,
                         ui inHeight,
                         ImageContext inImageContext) {
    CreateInfo info;
    info.mDevice       = &inDevice;
    info.mWidth        = inWidth;
    info.mHeight       = inHeight;
    info.mImageContext = inImageContext;
    Init(info);
}

VulkanImage::VulkanImage(VulkanDevice &inDevice,
                         VulkanSurface &inSurface,
                         ui inMSAASamples,
                         ImageContext inImageContext) {
    CreateInfo info;
    info.mDevice       = &inDevice;
    info.mSurface      = &inSurface;
    info.mMSAASamples  = inMSAASamples;
    info.mImageContext = inImageContext;
    Init(info);
}

VulkanImage::VulkanImage(VulkanDevice &inDevice,
                         VulkanSurface &inSurface,
                         vk::Image &inImage,
                         vk::ImageView &inImageView,
                         ImageContext inImageContext)
    : VulkanImageBase(inDevice) {
    mImageProperties.mExtent = inSurface.GetExtent();
    mImage                   = inImage;
    mImageView               = inImageView;
}

void VulkanImage::ExplicitDestroy() {
    if (mImage) {
        mDevice->waitIdle();
        mDevice->destroyImage(mImage);
        mDevice->destroyImageView(mImageView);
        mDevice->freeMemory(mDeviceMemory);
        mImage        = nullptr;
        mImageView    = nullptr;
        mDeviceMemory = nullptr;
    }
}

VulkanImageExternalHandled::VulkanImageExternalHandled(VulkanDevice &inDevice,
                                                       VulkanSurface &inSurface,
                                                       vk::Image inImage,
                                                       vk::ImageView inImageView)
    : VulkanImageBase(inDevice, false) {
    mImageProperties.mExtent = inSurface.GetExtent();
    mImage                   = inImage;
    mImageView               = inImageView;
}

void VulkanImage::Init(CreateInfo inCreateInfo) {
    VulkanImageBase::Init({.mDevice = inCreateInfo.mDevice});
    FillImageProperties(inCreateInfo.mImageContext, inCreateInfo.mMSAASamples);
    if (inCreateInfo.mSurface) {
        mImageProperties.mExtent = inCreateInfo.mSurface->GetExtent();
    }
    if (inCreateInfo.mWidth.has_value() and inCreateInfo.mHeight.has_value()) {
        mImageProperties.mExtent.setHeight(inCreateInfo.mHeight.value());
        mImageProperties.mExtent.setWidth(inCreateInfo.mWidth.value());
    }
    CreateImageAndBindMemory(mImage, mDeviceMemory);
    CreateImageView(mImage);
    mInitialized  = true;
    mVulkanDevice = inCreateInfo.mDevice;
}

void VulkanImage::Destroy() {
    ExplicitDestroy();
    mInitialized = false;
}

VulkanImage::~VulkanImage() {
    if (mInitialized) {
        Destroy();
    }
}

void VulkanImageBase::SetDebugUtilsName(s inS) {
    if (mVulkanDevice->GetVulkanInstance().IsValidationEnabled()) {
        auto vkSetDebugUtilsObjectNameEXT_ =
             (PFN_vkSetDebugUtilsObjectNameEXT)mVulkanDevice->GetVulkanInstance()
                  .GetInstanceHandle()
                  .getProcAddr("vkSetDebugUtilsObjectNameEXT");

        if (vkSetDebugUtilsObjectNameEXT_) {
            VkDebugUtilsObjectNameInfoEXT Info = {};
            Info.sType                         = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
            Info.objectType                    = VK_OBJECT_TYPE_IMAGE;
            Info.objectHandle                  = (uint64_t)(VkImage)mImage;
            Info.pObjectName                   = inS.c_str();
            vkSetDebugUtilsObjectNameEXT_((VkDevice)*mDevice, &Info);
        } else {
            assert(false && "Could not loadup the function vkSetDebugUtilsObjectNameEXT");
        }
    }
}