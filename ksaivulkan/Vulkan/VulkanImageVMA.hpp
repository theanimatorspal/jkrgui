#pragma once
#include "VulkanImage.hpp"
#include "VulkanVMA.hpp"

namespace ksai {
	template<ImageContext inImageContext>
	class VulkanImageVMA : public VulkanImageBase
	{
	public:
		VulkanImageVMA(const VulkanVMA& inVMA, const VulkanDevice& inDevice, uint32_t inWidth, uint32_t inHeight, uint32_t inChannel = 4)
			: VulkanImageBase(inDevice), mVMA(inVMA)
		{
			FillImageProperties<inImageContext>();
			// TODO inChannel
			mImageProperties.mExtent.width = inWidth;
			mImageProperties.mExtent.height = inHeight;
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
				mImageProperties.mImageUsage
			);

			auto AllocationCreateInfo = VmaAllocationCreateInfo();
			AllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
			VkImage Image_C;
			VkImageCreateInfo ImageCreateInfo_C = ImageCreateInfo;
			vmaCreateImage(inVMA.GetVMAHandle(), &ImageCreateInfo_C, &AllocationCreateInfo, &Image_C, &mAllocation, nullptr);
			mImage = Image_C;
			CreateImageView(mImage);
		}
		~VulkanImageVMA()
		{
			vmaDestroyImage(mVMA.GetVMAHandle(), mImage, mAllocation);
		}
	private:
		const VulkanVMA& mVMA;
		VmaAllocation mAllocation;
	};
}
