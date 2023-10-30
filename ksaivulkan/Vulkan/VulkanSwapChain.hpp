#pragma once
#include "VulkanQueueContext.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanImage.hpp"

namespace ksai {
	class VulkanSwapChainBase {
	public:
		VulkanSwapChainBase(const VulkanDevice& inDevice);
		~VulkanSwapChainBase();
		void ExplicitlyDestroy();
	public:
		using ImageResult = std::pair<uint32_t, uint32_t>;
		ImageResult AcquireNextImage(const VulkanSemaphore& inSemapore);
		static bool ImageIsNotOk(ImageResult inResult) { return (inResult.first != static_cast<uint32_t>(vk::Result::eSuccess)) and (inResult.first != static_cast<uint32_t>(vk::Result::eSuboptimalKHR)); }
		static bool ImageIsNotOptimal(ImageResult inResult) { return (inResult.second == static_cast<uint32_t>(vk::Result::eErrorOutOfDateKHR)) or (inResult.second == static_cast<uint32_t>(vk::Result::eSuboptimalKHR)); }
		static bool ImageIsNotOptimal(uint32_t inResult) { return (inResult == static_cast<uint32_t>(vk::Result::eErrorOutOfDateKHR)) or (inResult == static_cast<uint32_t>(vk::Result::eSuboptimalKHR)); }
	protected:
		const vk::Device& mDevice;
		vk::SwapchainKHR mSwapChain;
		std::vector<vk::Image> mSwapChainImages;
		std::vector<vk::ImageView> mSwapChainImageViews;
	};
}

namespace ksai {
	using VulkanImages = VulkanImage<ImageContext::ExternalHandled>;
	template<size_t inMaxFramesInFlight>
	class VulkanSwapChain : public VulkanSwapChainBase
	{
	public:
		VulkanSwapChain(const VulkanDevice& inDevice, const VulkanQueueContext& inQueueContext, const VulkanSurface& inSurface);
		~VulkanSwapChain() = default;
		VulkanSwapChain& operator=(VulkanSwapChain&& Other) noexcept {
			ExplicitlyDestroy();
			mSwapChain = std::move(Other.mSwapChain);
			mSwapChainImages = std::move(Other.mSwapChainImages);
			mSwapChainImageViews = std::move(Other.mSwapChainImageViews);
			Other.mSwapChain = nullptr;
			Other.mSwapChainImages.clear();
			Other.mSwapChainImageViews.clear();
			return *this;
		}
		GETTER& GetSwapChainImageViews() { return mSwapChainImageViews; }
		GETTER& GetSwapChainHandle() const { return mSwapChain; }
		std::vector<VulkanImages> GetVulkanImages(const VulkanDevice& inDevice, const VulkanSurface& inSurface);
	};
}

namespace ksai {
	template<size_t inMaxFramesInFlight>
	inline std::vector<VulkanImages> VulkanSwapChain<inMaxFramesInFlight>::GetVulkanImages(const VulkanDevice& inDevice, const VulkanSurface& inSurface)
	{
		std::vector<VulkanImages> Vectors;
		for (int i = 0; i < inMaxFramesInFlight; i++)
		{
			auto Image = VulkanImage<ImageContext::ExternalHandled>(inDevice, inSurface, nullptr, mSwapChainImageViews[i]);
			Vectors.push_back(std::move(Image));
		}
		return Vectors;
	}

	template<size_t inMaxFramesInFlight>
	VulkanSwapChain<inMaxFramesInFlight>::VulkanSwapChain(const VulkanDevice& inDevice, const VulkanQueueContext& inQueueContext, const VulkanSurface& inSurface) 
		: VulkanSwapChainBase(inDevice)
	{
		auto swapChainCreateInfo = vk::SwapchainCreateInfoKHR()
			.setFlags(vk::SwapchainCreateFlagsKHR())
			.setSurface(inSurface.GetSurfaceHandle())
			.setMinImageCount(std::clamp(static_cast<uint32_t>(inMaxFramesInFlight), inSurface.GetSurfaceCapabilities().minImageCount, inSurface.GetSurfaceCapabilities().maxImageCount))
			.setImageFormat(inSurface.GetSurfaceImageFormat())
			.setImageExtent(inSurface.GetExtent())
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
			.setImageSharingMode(vk::SharingMode::eExclusive)
			.setPreTransform(inSurface.GetPreTransform())
			.setCompositeAlpha(inSurface.GetCompositeAlpha())
			.setPresentMode(inSurface.GetPresentMode())
			.setClipped(true)
			.setImageArrayLayers(1);

		uint32_t queueFamilyIndices[2] = { inQueueContext.GetGraphicsQueueFamilyIndex(), inQueueContext.GetPresentQueueFamilyIndex() };
		if (queueFamilyIndices[0] != queueFamilyIndices[1])
		{
			swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			swapChainCreateInfo.queueFamilyIndexCount = 2;
			swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		}

		vk::SwapchainKHR swapChain = mDevice.createSwapchainKHR(swapChainCreateInfo);
		mSwapChain = swapChain;

		mSwapChainImages = mDevice.getSwapchainImagesKHR(mSwapChain);
		mSwapChainImageViews.resize(mSwapChainImages.size());
		auto Format = inSurface.GetSurfaceImageFormat();
		vk::ImageViewCreateInfo imageViewCreateInfo({}, {}, vk::ImageViewType::e2D, Format, {},
		{
			vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1
		}
		);
		for (int i = 0; i < mSwapChainImages.size(); i++)
		{
			imageViewCreateInfo.image = mSwapChainImages[i];
			mSwapChainImageViews[i] = mDevice.createImageView(imageViewCreateInfo);
		}
	}
}
