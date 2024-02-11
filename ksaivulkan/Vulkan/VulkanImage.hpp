#pragma once
#include "VulkanImageContext.hpp"
namespace ksai {
	class VulkanCommandBuffer;
}

namespace ksai {
	class VulkanImageBase {
	public:
		GETTER& GetImageFormat() const { return mImageProperties.mImageFormat; }
		GETTER& GetImageViewHandle() const { return mImageView; }
		GETTER& GetImageHandle() const { return mImage; }
		GETTER& GetImageExtent() const { return mImageProperties.mExtent; }
		GETTER GetInitialImageLayout() const { return mImageProperties.mInitialImageLayout; }
		GETTER GetAspect() const { return mImageProperties.mImageAspect; }
		GETTER GetImageProperty() const { return mImageProperties; }

		VulkanImageBase(const VulkanDevice& inDevice, bool inDestroyImageView = true);
		~VulkanImageBase();
		void SubmitImmediateCmdCopyFromData(const VulkanQueue<QueueContext::Graphics>& inQueue, const VulkanCommandBuffer& inCmdBuffer, const VulkanDevice& inDevice, void** inData, vk::DeviceSize inSize);
		void SubmitImmediateCmdCopyFromData(const VulkanQueue<QueueContext::Graphics>& inQueue, const VulkanCommandBuffer& inCmdBuffer, const VulkanDevice& inDevice, vk::DeviceSize inSize, std::span<void**> inLayerImageDatas);
		void CmdCopyImageFromImageAfterStage(const VulkanQueue<QueueContext::Graphics>& inQueue, const VulkanCommandBuffer& inCmdBuffer, const VulkanDevice& inDevice, VulkanImageBase& inImage, vk::PipelineStageFlags inAfterStage, vk::AccessFlags inAfterStageAccessFlags);
		void CmdTransitionImageLayout(const VulkanCommandBuffer& inBuffer, vk::ImageLayout inOldImageLayout, vk::ImageLayout inNewImageLayout, vk::PipelineStageFlags inBeforeStage, vk::PipelineStageFlags inAfterStage, vk::AccessFlags inBeforeAccess, vk::AccessFlags inAfterAccess) const;

	public:
		template <ImageContext inImageContext>
		void FillImageProperties(uint32_t inNumSamples = 1);
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
		bool mDestroyImageView = true;
	};
}

namespace ksai {
	template <ImageContext inImageContext>
	class VulkanImage : public VulkanImageBase {
	public:
		VulkanImage& operator=(VulkanImage&& Other) noexcept
		{
			ExplicitDestroy();
			MoveMembers(Other);
			return *this;
		}
		void MoveMembers(ksai::VulkanImage<inImageContext>& Other);
		VulkanImage();
		~VulkanImage()
		{
			ExplicitDestroy();
		}
		VulkanImage(const VulkanDevice& inDevice);
		VulkanImage(const VulkanDevice& inDevice, ui inWidth, ui inHeight);
		VulkanImage(const VulkanDevice& inDevice, const VulkanSurface& inSurface, ui inMSAASamples = 1);
		VulkanImage(const VulkanDevice& inDevice,
			const VulkanSurface& inSurface,
			const vk::Image& inImage,
			vk::ImageView& inImageView);

	private:
		void ExplicitDestroy();

	private:
		vk::DeviceMemory mDeviceMemory;
	};
}

namespace ksai {
	// TODO Rethink This
	template <>
	class VulkanImage<ImageContext::ExternalHandled> : public VulkanImageBase {
	public:
		VulkanImage(const VulkanDevice& inDevice, const VulkanSurface& inSurface, vk::Image inImage, vk::ImageView inImageView);
		~VulkanImage() = default;
	};
}

namespace ksai {
	template <ImageContext inImageContext>
	inline void VulkanImageBase::FillImageProperties(uint32_t inNumSamples)
	{
		// These are all for other than Defaults
		if constexpr (inImageContext == ImageContext::DepthImage) {
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
		else if constexpr (inImageContext == ImageContext::Storage) {
			mImageProperties.mImageUsage = vk::ImageUsageFlagBits::eStorage
				| vk::ImageUsageFlagBits::eTransferSrc;
			mImageProperties.mImageFormat = vk::Format::eR8G8B8A8Unorm;
		}
		else if constexpr (inImageContext == ImageContext::Default) {
			mImageProperties.mImageUsage = vk::ImageUsageFlagBits::eSampled
				| vk::ImageUsageFlagBits::eTransferDst;
			mImageProperties.mImageFormat = vk::Format::eR8G8B8A8Srgb;
		}
		else if constexpr (inImageContext == ImageContext::CubeCompatible) {
			mImageProperties.mFlags = vk::ImageCreateFlagBits::eCubeCompatible;
		}
		else if constexpr (inImageContext == ImageContext::ColorAttach)
		{
			mImageProperties.mImageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment;
			mImageProperties.mImageFormat = vk::Format::eR8G8B8A8Unorm; // This in on the surface, so
		}

		switch (inNumSamples)
		{
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
}

namespace ksai {
	template <ImageContext inImageContext>
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
	template <ImageContext inImageContext>
	inline VulkanImage<inImageContext>::VulkanImage()
	{
	}

	template <ImageContext inImageContext>
	inline VulkanImage<inImageContext>::VulkanImage(const VulkanDevice& inDevice)
		: VulkanImageBase(inDevice)
	{
		FillImageProperties<inImageContext>();
		CreateImageAndBindMemory(mImage, mDeviceMemory);
		CreateImageView(mImage);
	}

	template <ImageContext inImageContext>
	inline VulkanImage<inImageContext>::VulkanImage(const VulkanDevice& inDevice,
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

	template <ImageContext inImageContext>
	inline VulkanImage<inImageContext>::VulkanImage(const VulkanDevice& inDevice, const VulkanSurface& inSurface, ui inMSAASamples)
		: VulkanImageBase(inDevice)
	{
		FillImageProperties<inImageContext>(inMSAASamples);
		mImageProperties.mExtent = inSurface.GetExtent();
		CreateImageAndBindMemory(mImage, mDeviceMemory);
		CreateImageView(mImage);
	}

	template <ImageContext inImageContext>
	inline VulkanImage<inImageContext>::VulkanImage(const VulkanDevice& inDevice,
		const VulkanSurface& inSurface,
		const vk::Image& inImage,
		vk::ImageView& inImageView)
		: VulkanImageBase(inDevice)
	{
		mImageProperties.mExtent = inSurface.GetExtent();
		mImage = inImage;
		mImageView = inImageView;
	}

	template <ImageContext inImageContext>
	inline void VulkanImage<inImageContext>::ExplicitDestroy()
	{
		if (mImage) {
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
	inline VulkanImage<ImageContext::ExternalHandled>::VulkanImage(const VulkanDevice& inDevice, const VulkanSurface& inSurface, vk::Image inImage, vk::ImageView inImageView)
		: VulkanImageBase(inDevice, false)
	{
		mImageProperties.mExtent = inSurface.GetExtent();
		mImage = inImage;
		mImageView = inImageView;
	}
}
