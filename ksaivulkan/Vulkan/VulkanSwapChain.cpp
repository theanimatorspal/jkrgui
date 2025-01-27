#include "VulkanSwapChain.hpp"
using namespace ksai;

ksai::VulkanSwapChainBase::VulkanSwapChainBase(VulkanDevice &inDevice) { Init({&inDevice}); }

void VulkanSwapChainBase::Init(CreateInfo inCreateInfo) {
    mDevice      = &inCreateInfo.mDevice->GetDeviceHandle();
    mInitialized = true;
}

void ksai::VulkanSwapChainBase::ExplicitlyDestroy() {
    if (mSwapChain) {
        mDevice->waitIdle();
        for (auto &u : mSwapChainImageViews) {
            mDevice->destroyImageView(u);
        }
        mDevice->destroy(mSwapChain);
        mSwapChainImageViews.clear();
        mSwapChainImages.clear();
        mSwapChain = nullptr;
    }
}

void ksai::VulkanSwapChainBase::ExplicitlyDestroyOldSwapChain() {
    if (mOldSwapChain) {
        mDevice->waitIdle();
        for (auto &u : mOldSwapChainImageViews) {
            mDevice->destroyImageView(u);
        }
        mDevice->destroy(mOldSwapChain);
        mOldSwapChainImages.clear();
        mOldSwapChainImageViews.clear();
        mOldSwapChain = nullptr;
    }
}

std::pair<uint32_t, uint32_t>
ksai::VulkanSwapChainBase::AcquireNextImage(VulkanSemaphore &inSemapore) {
    vk::ResultValue<uint32_t> Result = mDevice->acquireNextImageKHR(
         mSwapChain, std::numeric_limits<uint32_t>::max(), inSemapore.GetSemaphoreHandle());
    return std::pair<uint32_t, uint32_t>(static_cast<uint32_t>(Result.result),
                                         static_cast<uint32_t>(Result.value));
}

v<VulkanImages> VulkanSwapChain::GetVulkanImages(VulkanDevice &inDevice, VulkanSurface &inSurface) {
    v<VulkanImages> Vectors;
    for (int i = 0; i < mSwapChainImageViews.size(); i++) {
        auto Image = VulkanImageExternalHandled(
             inDevice, inSurface, mSwapChainImages[i], mSwapChainImageViews[i]);
        Vectors.push_back(std::move(Image));
    }
    return Vectors;
}

VulkanSwapChain::VulkanSwapChain(VulkanDevice &inDevice,
                                 VulkanQueueContext &inQueueContext,
                                 VulkanSurface &inSurface,
                                 optref<VulkanSwapChainBase> inOldSwapChain,
                                 sz inMaxFramesInFlight) {
    Init({&inDevice, &inQueueContext, &inSurface, inOldSwapChain, inMaxFramesInFlight});
}

void VulkanSwapChain::Init(CreateInfo inCreateInfo) {
    VulkanSwapChainBase::Init({inCreateInfo.mDevice});
    auto &inSurface          = *inCreateInfo.mSurface;
    auto inMaxFramesInFlight = inCreateInfo.mMaxFramesInFlight;
    auto &inOldSwapChain     = inCreateInfo.mOldSwapChain;
    auto &inQueueContext     = *inCreateInfo.mQueueContext;
    auto swapChainCreateInfo =
         vk::SwapchainCreateInfoKHR()
              .setFlags(vk::SwapchainCreateFlagsKHR())
              .setSurface(inSurface.GetSurfaceHandle())
              .setMinImageCount(std::clamp(static_cast<ui>(inMaxFramesInFlight),
                                           inSurface.GetSurfaceCapabilities().minImageCount,
                                           inSurface.GetSurfaceCapabilities().maxImageCount))
              .setImageFormat(inSurface.GetSurfaceImageFormat())
              .setImageExtent(inSurface.GetExtent())
              //.setImageColorSpace(inSurface.GetColorSpace())
              .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment |
                             vk::ImageUsageFlagBits::eTransferDst)
              .setImageSharingMode(vk::SharingMode::eExclusive)
              .setPreTransform(inSurface.GetPreTransform())
              .setCompositeAlpha(inSurface.GetCompositeAlpha())
              .setPresentMode(inSurface.GetPresentMode())
              .setClipped(true)
              .setImageArrayLayers(1);
    if (inOldSwapChain.has_value()) {
        ExplicitlyDestroyOldSwapChain();
        swapChainCreateInfo.setOldSwapchain(inOldSwapChain.value().get().GetSwapChainHandle());
        auto &oldSwapChain                = inOldSwapChain.value().get();
        mOldSwapChain                     = std::move(oldSwapChain.GetSwapChainHandle());
        mOldSwapChainImages               = std::move(oldSwapChain.GetSwapChainImages());
        mOldSwapChainImageViews           = std::move(oldSwapChain.GetSwapChainImageViews());
        oldSwapChain.GetSwapChainHandle() = nullptr;
    }

    ui queueFamilyIndices[2] = {inQueueContext.GetGraphicsQueueFamilyIndex(),
                                inQueueContext.GetPresentQueueFamilyIndex()};
    if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
        swapChainCreateInfo.imageSharingMode      = vk::SharingMode::eConcurrent;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices   = queueFamilyIndices;
        swapChainCreateInfo.imageUsage |= vk::ImageUsageFlagBits::eTransferDst;
    }

    vk::SwapchainKHR swapChain = mDevice->createSwapchainKHR(swapChainCreateInfo);
    mSwapChain                 = swapChain;

    mSwapChainImages           = mDevice->getSwapchainImagesKHR(mSwapChain);
    mSwapChainImageViews.resize(mSwapChainImages.size());
    auto Format                                   = inSurface.GetSurfaceImageFormat();
    vk::ImageSubresourceRange imgSubResourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};
    vk::ImageViewCreateInfo imageViewCreateInfo(
         {}, {}, vk::ImageViewType::e2D, Format, {}, imgSubResourceRange);
    for (int i = 0; i < mSwapChainImages.size(); i++) {
        imageViewCreateInfo.image = mSwapChainImages[i];
        mSwapChainImageViews[i]   = mDevice->createImageView(imageViewCreateInfo);
    }
    mInitialized = true;
}

void VulkanSwapChain::Destroy() {
    ExplicitlyDestroyOldSwapChain();
    ExplicitlyDestroy();
    mInitialized = false;
};

ksai::VulkanSwapChain::~VulkanSwapChain() {
    if (mInitialized) {
        Destroy();
    }
}