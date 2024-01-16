#pragma once
#include "VulkanDevice.hpp"
#include "VulkanImage.hpp"
#include "VulkanQueueContext.hpp"
#include "VulkanSemaphore.hpp"

namespace ksai {
class VulkanSwapChainBase {
public:
    VulkanSwapChainBase(const VulkanDevice& inDevice);
    ~VulkanSwapChainBase();
    void ExplicitlyDestroy();
    void ExplicitlyDestroyOldSwapChain();

public:
    using ImageResult = p<ui, ui>;
    ImageResult AcquireNextImage(const VulkanSemaphore& inSemapore);
    static bool ImageIsNotOk(ImageResult inResult) { return (inResult.first != static_cast<ui>(vk::Result::eSuccess)) and (inResult.first != static_cast<ui>(vk::Result::eSuboptimalKHR)); }
    static bool ImageIsNotOptimal(ImageResult inResult) { return (inResult.second == static_cast<ui>(vk::Result::eErrorOutOfDateKHR)) or (inResult.second == static_cast<ui>(vk::Result::eSuboptimalKHR)); }
    static bool ImageIsNotOptimal(ui inResult) { return (inResult == static_cast<ui>(vk::Result::eErrorOutOfDateKHR)) or (inResult == static_cast<ui>(vk::Result::eSuboptimalKHR)); }

    GETTER& GetSwapChainHandle() { return mSwapChain; }
    GETTER& GetSwapChainImages() { return mSwapChainImages; }
    GETTER& GetSwapChainImageViews() { return mSwapChainImageViews; }

protected:
    const vk::Device& mDevice;
    vk::SwapchainKHR mSwapChain;
    v<vk::Image> mSwapChainImages;
    v<vk::ImageView> mSwapChainImageViews;

    vk::SwapchainKHR mOldSwapChain = nullptr;
    v<vk::Image> mOldSwapChainImages;
    v<vk::ImageView> mOldSwapChainImageViews;
};
}

namespace ksai {
using VulkanImages = VulkanImage<ImageContext::ExternalHandled>;
template <sz inMaxFramesInFlight>
class VulkanSwapChain : public VulkanSwapChainBase {
public:
    VulkanSwapChain(const VulkanDevice& inDevice, const VulkanQueueContext& inQueueContext, const VulkanSurface& inSurface, optref<VulkanSwapChainBase> inOldSwapChain = std::nullopt);
    ~VulkanSwapChain() = default;
    VulkanSwapChain& operator=(VulkanSwapChain&& Other) noexcept
    {
        ExplicitlyDestroy();
        ExplicitlyDestroyOldSwapChain();

        mSwapChain = std::move(Other.mSwapChain);
        mSwapChainImages = std::move(Other.mSwapChainImages);
        mSwapChainImageViews = std::move(Other.mSwapChainImageViews);
        Other.mSwapChain = nullptr;
        Other.mSwapChainImages.clear();
        Other.mSwapChainImageViews.clear();

        mOldSwapChain = std::move(Other.mOldSwapChain);
        mOldSwapChainImages = std::move(Other.mOldSwapChainImages);
        mOldSwapChainImageViews = std::move(Other.mOldSwapChainImageViews);
        Other.mOldSwapChain = nullptr;
        Other.mOldSwapChainImages.clear();
        Other.mOldSwapChainImageViews.clear();

        return *this;
    }
    GETTER& GetSwapChainImageViews() { return mSwapChainImageViews; }
    GETTER& GetSwapChainHandle() const { return mSwapChain; }
    v<VulkanImages> GetVulkanImages(const VulkanDevice& inDevice, const VulkanSurface& inSurface);
};
}

namespace ksai {
template <sz inMaxFramesInFlight>
inline v<VulkanImages> VulkanSwapChain<inMaxFramesInFlight>::GetVulkanImages(const VulkanDevice& inDevice, const VulkanSurface& inSurface)
{
    v<VulkanImages> Vectors;
    for (int i = 0; i < inMaxFramesInFlight; i++) {
        auto Image = VulkanImage<ImageContext::ExternalHandled>(inDevice, inSurface, nullptr, mSwapChainImageViews[i]);
        Vectors.push_back(std::move(Image));
    }
    return Vectors;
}

template <sz inMaxFramesInFlight>
VulkanSwapChain<inMaxFramesInFlight>::VulkanSwapChain(const VulkanDevice& inDevice, const VulkanQueueContext& inQueueContext, const VulkanSurface& inSurface, optref<VulkanSwapChainBase> inOldSwapChain)
    : VulkanSwapChainBase(inDevice)
{
    auto swapChainCreateInfo = vk::SwapchainCreateInfoKHR()
                                   .setFlags(vk::SwapchainCreateFlagsKHR())
                                   .setSurface(inSurface.GetSurfaceHandle())
                                   .setMinImageCount(std::clamp(static_cast<ui>(inMaxFramesInFlight), inSurface.GetSurfaceCapabilities().minImageCount, inSurface.GetSurfaceCapabilities().maxImageCount))
                                   .setImageFormat(inSurface.GetSurfaceImageFormat())
                                   .setImageExtent(inSurface.GetExtent())
                                   .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                                   .setImageSharingMode(vk::SharingMode::eExclusive)
                                   .setPreTransform(inSurface.GetPreTransform())
                                   .setCompositeAlpha(inSurface.GetCompositeAlpha())
                                   .setPresentMode(inSurface.GetPresentMode())
                                   .setClipped(true)
                                   .setImageArrayLayers(1);
    if (inOldSwapChain.has_value()) {
        ExplicitlyDestroyOldSwapChain();
        swapChainCreateInfo.setOldSwapchain(inOldSwapChain.value().get().GetSwapChainHandle());
        auto& oldSwapChain = inOldSwapChain.value().get();
        mOldSwapChain = std::move(oldSwapChain.GetSwapChainHandle());
        mOldSwapChainImages = std::move(oldSwapChain.GetSwapChainImages());
        mOldSwapChainImageViews = std::move(oldSwapChain.GetSwapChainImageViews());
        oldSwapChain.GetSwapChainHandle() = nullptr;
    }

    ui queueFamilyIndices[2] = { inQueueContext.GetGraphicsQueueFamilyIndex(), inQueueContext.GetPresentQueueFamilyIndex() };
    if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
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
        { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });
    for (int i = 0; i < mSwapChainImages.size(); i++) {
        imageViewCreateInfo.image = mSwapChainImages[i];
        mSwapChainImageViews[i] = mDevice.createImageView(imageViewCreateInfo);
    }
}
}
