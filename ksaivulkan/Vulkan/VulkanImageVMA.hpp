#pragma once
#include "VulkanImage.hpp"
#include "VulkanVMA.hpp"

namespace ksai {
template <ImageContext inImageContext> class VulkanImageVMA : public VulkanImageBase {
    public:
    VulkanImageVMA(const VulkanVMA& inVMA,
                   const VulkanDevice& inDevice,
                   ui inWidth,
                   ui inHeight,
                   ui inChannel    = 4,
                   ui inLayerCount = 1,
                   ui inSamples    = 1)
        : VulkanImageBase(inDevice), mVMA(inVMA) {
        FillImageProperties(inImageContext, inSamples);
        mImageProperties.mArrayLayers   = inLayerCount;
        mImageProperties.mExtent.width  = inWidth;
        mImageProperties.mExtent.height = inHeight;
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
    ~VulkanImageVMA() {
        mDevice.waitIdle();
        vmaDestroyImage(mVMA.GetVMAHandle(), mImage, mAllocation);
    }

    private:
    const VulkanVMA& mVMA;
    VmaAllocation mAllocation;
};
} // namespace ksai
