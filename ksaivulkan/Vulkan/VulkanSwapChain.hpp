#pragma once
#include "VulkanDevice.hpp"
#include "VulkanImage.hpp"
#include "VulkanQueueContext.hpp"
#include "VulkanSemaphore.hpp"

namespace ksai {
class VulkanBufferBase;
class VulkanSwapChainBase {
    public:
    struct CreateInfo {
        const VulkanDevice *mDevice;
    };

    VulkanSwapChainBase()                                            = default;
    VulkanSwapChainBase(const VulkanSwapChainBase &other)            = default;
    VulkanSwapChainBase &operator=(const VulkanSwapChainBase &other) = default;
    VulkanSwapChainBase(VulkanSwapChainBase &&other)                 = default;
    VulkanSwapChainBase &operator=(VulkanSwapChainBase &&other)      = default;
    operator vk::SwapchainKHR() const { return mSwapChain; }

    void Init(CreateInfo inCreateInfo);

    VulkanSwapChainBase(const VulkanDevice &inDevice);
    ~VulkanSwapChainBase() = default;
    void ExplicitlyDestroy();
    void ExplicitlyDestroyOldSwapChain();

    public:
    using ImageResult = p<ui, ui>;
    ImageResult AcquireNextImage(const VulkanSemaphore &inSemapore);
    static bool ImageIsNotOk(ImageResult inResult) {
        return (inResult.first != static_cast<ui>(vk::Result::eSuccess)) and
               (inResult.first != static_cast<ui>(vk::Result::eSuboptimalKHR));
    }
    static bool ImageIsNotOptimal(ImageResult inResult) {
        return (inResult.second == static_cast<ui>(vk::Result::eErrorOutOfDateKHR)) or
               (inResult.second == static_cast<ui>(vk::Result::eSuboptimalKHR));
    }
    static bool ImageIsNotOptimal(ui inResult) {
        return (inResult == static_cast<ui>(vk::Result::eErrorOutOfDateKHR)) or
               (inResult == static_cast<ui>(vk::Result::eSuboptimalKHR));
    }

    GETTER &GetSwapChainHandle() const { return mSwapChain; }
    GETTER &GetSwapChainHandle() { return mSwapChain; }
    GETTER &GetSwapChainImages() { return mSwapChainImages; }
    GETTER &GetSwapChainImageViews() { return mSwapChainImageViews; }

    protected:
    const vk::Device *mDevice;
    vk::SwapchainKHR mSwapChain;
    vk::Extent2D mSwapChainImageExtent;
    v<vk::Image> mSwapChainImages;
    v<vk::ImageView> mSwapChainImageViews;

    vk::SwapchainKHR mOldSwapChain = nullptr;
    v<vk::Image> mOldSwapChainImages;
    v<vk::ImageView> mOldSwapChainImageViews;
    bool mInitialized = false;
};
} // namespace ksai

namespace ksai {
using VulkanImages = VulkanImageExternalHandled;
class VulkanSwapChain : public VulkanSwapChainBase {
    public:
    struct CreateInfo {
        const VulkanDevice *mDevice             = nullptr;
        const VulkanQueueContext *mQueueContext = nullptr;
        const VulkanSurface *mSurface           = nullptr;
        optref<VulkanSwapChainBase> mOldSwapChain;
        sz mMaxFramesInFlight = 2;
    };

    VulkanSwapChain() = default;
    ~VulkanSwapChain();
    VulkanSwapChain(const VulkanSwapChain &other)            = delete;
    VulkanSwapChain &operator=(const VulkanSwapChain &other) = delete;
    VulkanSwapChain(VulkanSwapChain &&other)                 = default;
    VulkanSwapChain &operator=(VulkanSwapChain &&Other) noexcept {
        ExplicitlyDestroy();
        ExplicitlyDestroyOldSwapChain();

        mSwapChain           = std::move(Other.mSwapChain);
        mSwapChainImages     = std::move(Other.mSwapChainImages);
        mSwapChainImageViews = std::move(Other.mSwapChainImageViews);
        Other.mSwapChain     = nullptr;
        Other.mSwapChainImages.clear();
        Other.mSwapChainImageViews.clear();

        mOldSwapChain           = std::move(Other.mOldSwapChain);
        mOldSwapChainImages     = std::move(Other.mOldSwapChainImages);
        mOldSwapChainImageViews = std::move(Other.mOldSwapChainImageViews);
        Other.mOldSwapChain     = nullptr;
        Other.mOldSwapChainImages.clear();
        Other.mOldSwapChainImageViews.clear();

        return *this;
    }

    void Init(CreateInfo inCreateInfo);
    void Destroy();

    v<VulkanImages> GetVulkanImages(const VulkanDevice &inDevice, const VulkanSurface &inSurface);

    VulkanSwapChain(const VulkanDevice &inDevice,
                    const VulkanQueueContext &inQueueContext,
                    const VulkanSurface &inSurface,
                    optref<VulkanSwapChainBase> inOldSwapChain = std::nullopt,
                    sz inMaxFramesInFlight                     = 2);

    private:
    bool mInitialized = false;
};
} // namespace ksai