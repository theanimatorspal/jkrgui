#pragma once
#include <vulkan/vulkan.hpp>
#include "VulkanDevice.hpp"
#include "VulkanSurface.hpp"
#include "VulkanQueue.hpp"
#include <iostream>

namespace ksai {
	class VulkanCommandBuffer;

	enum class ImageContext
	{
		Default,
		Unknown,
		ExternalHandled,
		DepthImage,
		Storage,
	};

	struct ImageProperties
	{
		vk::Format mImageFormat = vk::Format::eR16G16B16A16Sfloat;
		vk::FormatFeatureFlagBits mImageFormatFeature = vk::FormatFeatureFlagBits::eSampledImage;
		vk::ImageType mImageType = vk::ImageType::e2D;
		vk::ImageUsageFlags mImageUsage = vk::ImageUsageFlagBits::eSampled;
		vk::SampleCountFlagBits mSampleCountFlagBits = vk::SampleCountFlagBits::e1;
		ui mMipLevels = 1;
		ui mArrayLayers = 1;
		vk::ImageTiling mTiling = vk::ImageTiling::eOptimal;
		vk::ImageAspectFlagBits mImageAspect = vk::ImageAspectFlagBits::eColor;
		vk::MemoryPropertyFlagBits mMemoryProperty = vk::MemoryPropertyFlagBits::eDeviceLocal;
		vk::ImageViewType mImageViewType = vk::ImageViewType::e2D;
		vk::SharingMode mSharingMode = vk::SharingMode::eExclusive;
		vk::ImageLayout mInitialImageLayout = vk::ImageLayout::eGeneral;
		vk::Extent2D mExtent;
	};
}

namespace ksai {
	class VulkanImageBase
	{
	public:
		VulkanImageBase(const VulkanDevice& inDevice);
		~VulkanImageBase();
		void SubmitImmediateCmdCopyFromData(const VulkanQueue<QueueContext::Graphics>& inQueue, const VulkanCommandBuffer& inCmdBuffer, const VulkanDevice& inDevice, void** inData, vk::DeviceSize inSize);
		void CmdTransitionImageLayout(const VulkanCommandBuffer& inBuffer, vk::ImageLayout inOldImageLayout, vk::ImageLayout inNewImageLayout, vk::PipelineStageFlags inBeforeStage, vk::PipelineStageFlags inAfterStage, vk::AccessFlags inBeforeAccess, vk::AccessFlags inAfterAccess);
		GETTER& GetImageFormat() const { return mImageProperties.mImageFormat; }
		GETTER& GetImageViewHandle() const { return mImageView; }
		GETTER& GetImageHandle() const { return mImage; }
		GETTER& GetImageExtent() const { return mImageProperties.mExtent; }
		GETTER GetInitialImageLayout() const { return mImageProperties.mInitialImageLayout; }
		GETTER GetAspect() const { return mImageProperties.mImageAspect; }
        GETTER GetImageProperty() const { return mImageProperties; }

    public:
		template<ImageContext inImageContext>
		void FillImageProperties();
		void GetMemoryTypeIndex(vk::MemoryPropertyFlagBits inFlag, vk::Image inImage, vk::DeviceSize& outSize, ui& outIndex);
		void GetImageTiling(vk::Format inFormat, vk::FormatFeatureFlagBits inFormatFeature, vk::ImageTiling& outTiling);
	protected:
		void CreateImageAndBindMemory(vk::Image& inImage, vk::DeviceMemory& inDeviceMemory);
		void CreateImageView(vk::Image& inImage);
	protected:
		const vk::PhysicalDevice& mPhysicalDevice;
		const vk::Device& mDevice;
		vk::Image mImage = nullptr;
		vk::ImageView mImageView = nullptr;
		ImageProperties mImageProperties;
	};
}

namespace ksai {
	template<ImageContext inImageContext>
	class VulkanImage : public VulkanImageBase
	{
	public:
		VulkanImage& operator=(VulkanImage&& Other) noexcept
		{
			ExplicitDestroy();
			MoveMembers(Other);
			return *this;
		}
		void MoveMembers(ksai::VulkanImage<inImageContext>& Other);
		VulkanImage();
		~VulkanImage() {
			ExplicitDestroy();
		}
		VulkanImage(const VulkanDevice& inDevice);
		VulkanImage(const VulkanDevice& inDevice, ui inWidth, ui inHeight);
		VulkanImage(const VulkanDevice& inDevice, const VulkanSurface& inSurface);
        VulkanImage(const VulkanDevice &inDevice,
                    const VulkanSurface &inSurface,
                    const vk::Image &inImage,
                    vk::ImageView &inImageView);

    private:
        void ExplicitDestroy();

    private:
        vk::DeviceMemory mDeviceMemory;
    };
}

namespace ksai {
	template<>
	class VulkanImage<ImageContext::ExternalHandled>
	{
	public:
		VulkanImage(const VulkanDevice& inDevice, const VulkanSurface& inSurface, const vk::Image& inImage, vk::ImageView& inImageView);
		~VulkanImage() = default;
		inline constexpr const auto& GetImageViewHandle() const { return mImageView; }
		inline constexpr const auto& GetImageExtent() const { return mExtent; }
	private:
		vk::Image mImage;
		vk::Extent2D mExtent;
		vk::ImageView mImageView;
	};
}

namespace ksai {
	template<ImageContext inImageContext>
	inline void VulkanImageBase::FillImageProperties()
	{
		if constexpr (inImageContext == ImageContext::DepthImage)
		{
			mImageProperties.mImageFormat = vk::Format::eD16Unorm;
			mImageProperties.mImageFormatFeature = vk::FormatFeatureFlagBits::eDepthStencilAttachment;
			mImageProperties.mImageType = vk::ImageType::e2D;
			mImageProperties.mSampleCountFlagBits = vk::SampleCountFlagBits::e1;
			mImageProperties.mImageUsage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
			mImageProperties.mImageAspect = vk::ImageAspectFlagBits::eDepth;
			mImageProperties.mMemoryProperty = vk::MemoryPropertyFlagBits::eDeviceLocal;
			mImageProperties.mImageViewType = vk::ImageViewType::e2D;
			mImageProperties.mInitialImageLayout = vk::ImageLayout::eDepthAttachmentOptimal;
		}
		else if constexpr (inImageContext == ImageContext::Storage)
		{
            mImageProperties.mImageUsage = vk::ImageUsageFlagBits::eStorage
                                           | vk::ImageUsageFlagBits::eTransferSrc;
            mImageProperties.mImageFormat = vk::Format::eR8G8B8A8Unorm;
        } else if constexpr (inImageContext == ImageContext::Default) {
            mImageProperties.mImageUsage = vk::ImageUsageFlagBits::eSampled
                                           | vk::ImageUsageFlagBits::eTransferDst;
            mImageProperties.mImageFormat = vk::Format::eR8G8B8A8Srgb;
        }
    }
}

namespace ksai {
	template<ImageContext inImageContext>
	inline void VulkanImage<inImageContext>::MoveMembers(ksai::VulkanImage<inImageContext>& Other)
	{
		mImage = std::move(Other.mImage);
		mImageView = std::move(Other.mImageView);
		mDeviceMemory = std::move(Other.mDeviceMemory);
		mImageProperties.mExtent = std::move(Other.mImageProperties.mExtent);
		Other.mImage = nullptr;
		Other.mImageView = nullptr;
		Other.mDeviceMemory = nullptr;
	}
	template<ImageContext inImageContext>
	inline VulkanImage<inImageContext>::VulkanImage()
	{
	}

    template<ImageContext inImageContext>
    inline VulkanImage<inImageContext>::VulkanImage(const VulkanDevice &inDevice)
        : VulkanImageBase(inDevice)
    {
        FillImageProperties<inImageContext>();
        CreateImageAndBindMemory(mImage, mDeviceMemory);
		CreateImageView(mImage);
    }

    template<ImageContext inImageContext>
    inline VulkanImage<inImageContext>::VulkanImage(const VulkanDevice &inDevice,
                                                    ui inWidth,
                                                    ui inHeight)
        : VulkanImageBase(inDevice)
    {
		FillImageProperties<inImageContext>();
		mImageProperties.mExtent.width = inWidth;
		mImageProperties.mExtent.height = inHeight;
		CreateImageAndBindMemory(mImage, mDeviceMemory);
		CreateImageView(mImage);
	}

	template<ImageContext inImageContext>
	inline VulkanImage<inImageContext>::VulkanImage(const VulkanDevice& inDevice, const VulkanSurface& inSurface)
		: VulkanImageBase(inDevice)
	{
		FillImageProperties<inImageContext>();
		mImageProperties.mExtent = inSurface.GetExtent();
		CreateImageAndBindMemory(mImage, mDeviceMemory);
		CreateImageView(mImage);
	}

    template<ImageContext inImageContext>
    inline VulkanImage<inImageContext>::VulkanImage(const VulkanDevice &inDevice,
                                                    const VulkanSurface &inSurface,
                                                    const vk::Image &inImage,
                                                    vk::ImageView &inImageView)
        : VulkanImageBase(inDevice)
    {
		mImageProperties.mExtent = inSurface.GetExtent();
		mImage = inImage;
		mImageView = inImageView;
	}

	template<ImageContext inImageContext>
	inline void VulkanImage<inImageContext>::ExplicitDestroy()
	{
		if (mImage)
		{
			mDevice.waitIdle();
			mDevice.destroyImage(mImage);
			mDevice.destroyImageView(mImageView);
			mDevice.freeMemory(mDeviceMemory);
			mImage = nullptr;
			mImageView = nullptr;
			mDeviceMemory = nullptr;
		}
	}


}

namespace ksai {
	inline VulkanImage<ImageContext::ExternalHandled>::VulkanImage(const VulkanDevice& inDevice, const VulkanSurface& inSurface, const vk::Image& inImage, vk::ImageView& inImageView)
	{
		mExtent = inSurface.GetExtent();
		mImage = inImage;
		mImageView = inImageView;
	}
}

