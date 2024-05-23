#include "VulkanImageVMA.hpp"

using namespace ksai;
VulkanImageVMA::VulkanImageVMA(const VulkanVMA& inVMA,
                               const VulkanDevice& inDevice,
                               ui inWidth,
                               ui inHeight,
                               ImageContext inImageContext,
                               ui inChannel,
                               ui inLayerCount,
                               ui inSamples,
                               ui inMips,
                               opt<vk::ImageUsageFlags> inBits)
    : VulkanImageBase(inDevice), mVMA(inVMA) {
    FillImageProperties(inImageContext, inSamples);
    mImageProperties.mArrayLayers   = inLayerCount;
    mImageProperties.mExtent.width  = inWidth;
    mImageProperties.mExtent.height = inHeight;
    mImageProperties.mMipLevels     = inMips;
    if (inBits.has_value()) {
        mImageProperties.mImageUsage |= inBits.value();
    }
    vk::ImageTiling Tiling;
    GetImageTiling(mImageProperties.mImageFormat, mImageProperties.mImageFormatFeature, Tiling);
    auto ImageCreateInfo       = vk::ImageCreateInfo(mImageProperties.mFlags,
                                               mImageProperties.mImageType,
                                               mImageProperties.mImageFormat,
                                               vk::Extent3D(mImageProperties.mExtent, 1),
                                               mImageProperties.mMipLevels,
                                               mImageProperties.mArrayLayers,
                                               mImageProperties.mSampleCountFlagBits,
                                               mImageProperties.mTiling,
                                               mImageProperties.mImageUsage);

    auto AllocationCreateInfo  = VmaAllocationCreateInfo();
    AllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    VkImage Image_C;
    VkImageCreateInfo ImageCreateInfo_C = ImageCreateInfo;
    vmaCreateImage(inVMA.GetVMAHandle(),
                   &ImageCreateInfo_C,
                   &AllocationCreateInfo,
                   &Image_C,
                   &mAllocation,
                   nullptr);
    mImage = Image_C;
    CreateImageView(mImage);
}

VulkanImageVMA::~VulkanImageVMA() {
    mDevice.waitIdle();
    vmaDestroyImage(mVMA.GetVMAHandle(), mImage, mAllocation);
}
