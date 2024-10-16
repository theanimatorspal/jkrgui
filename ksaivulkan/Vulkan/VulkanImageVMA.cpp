#include "VulkanImageVMA.hpp"

using namespace ksai;
VulkanImageVMA::VulkanImageVMA(VulkanVMA &inVMA,
                               VulkanDevice &inDevice,
                               ui inWidth,
                               ui inHeight,
                               ImageContext inImageContext,
                               ui inChannel,
                               ui inLayerCount,
                               ui inSamples,
                               ui inMips,
                               opt<vk::ImageUsageFlags> inBits,
                               opt<vk::ImageLayout> inLayout,
                               opt<vk::Format> inFormat) {
    Init({.inVMA          = &inVMA,
          .inDevice       = &inDevice,
          .inWidth        = inWidth,
          .inHeight       = inHeight,
          .inImageContext = inImageContext,
          .inChannel      = inChannel,
          .inLayerCount   = inLayerCount,
          .inSamples      = inSamples,
          .inMips         = inMips,
          .inUsageBits    = inBits,
          .inLayout       = inLayout,
          .inFormat       = inFormat});
}

VulkanImageVMA::~VulkanImageVMA() {
    if (mInitialized) {
        Destroy();
    }
}

void VulkanImageVMA::Init(CreateInfo inCreateInfo) {
    VulkanImageBase::Init({.mDevice = inCreateInfo.inDevice});
    mVMA                            = inCreateInfo.inVMA;
    mDevice                         = &inCreateInfo.inDevice->GetDeviceHandle();
    mImageProperties.mArrayLayers   = inCreateInfo.inLayerCount;
    mImageProperties.mExtent.width  = inCreateInfo.inWidth;
    mImageProperties.mExtent.height = inCreateInfo.inHeight;
    mImageProperties.mMipLevels     = inCreateInfo.inMips;
    FillImageProperties(inCreateInfo.inImageContext, inCreateInfo.inSamples);
    if (inCreateInfo.inUsageBits.has_value()) {
        mImageProperties.mImageUsage |= inCreateInfo.inUsageBits.value();
    }
    if (inCreateInfo.inLayout.has_value()) {
        mImageProperties.mCurrentImageLayout = inCreateInfo.inLayout.value();
        mImageProperties.mInitialImageLayout = inCreateInfo.inLayout.value();
    }
    if (inCreateInfo.inFormat.has_value()) {
        mImageProperties.mImageFormat = inCreateInfo.inFormat.value();
    }
    if (inCreateInfo.inImageViewType.has_value()) {
        mImageProperties.mImageViewType = inCreateInfo.inImageViewType.value();
    }
    if (inCreateInfo.inImageProperties.has_value()) {
        mImageProperties = inCreateInfo.inImageProperties.value();
    }
    vk::ImageTiling Tiling;
    GetImageTiling(mImageProperties.mImageFormat, mImageProperties.mImageFormatFeature, Tiling);
    auto ImageCreateInfo = vk::ImageCreateInfo(mImageProperties.mFlags,
                                               mImageProperties.mImageType,
                                               mImageProperties.mImageFormat,
                                               vk::Extent3D(mImageProperties.mExtent, 1),
                                               mImageProperties.mMipLevels,
                                               mImageProperties.mArrayLayers,
                                               mImageProperties.mSampleCountFlagBits,
                                               mImageProperties.mTiling,
                                               mImageProperties.mImageUsage)
                                .setInitialLayout(vk::ImageLayout::eUndefined);

    auto AllocationCreateInfo  = VmaAllocationCreateInfo();
    AllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    VkImage Image_C;
    VkImageCreateInfo ImageCreateInfo_C = ImageCreateInfo;
    vmaCreateImage(inCreateInfo.inVMA->GetVMAHandle(),
                   &ImageCreateInfo_C,
                   &AllocationCreateInfo,
                   &Image_C,
                   &mAllocation,
                   nullptr);
    mImage = Image_C;
    CreateImageView(mImage);
    mInitialized = true;
}

void VulkanImageVMA::Destroy() {
    mDevice->waitIdle();
    vmaDestroyImage(mVMA->GetVMAHandle(), mImage, mAllocation);
    mInitialized = false;
}