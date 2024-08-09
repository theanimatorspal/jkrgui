#pragma once
#include "PainterParameter.hpp"
#include "SDLWindow.hpp"
#include <VulkanCommandBuffer.hpp>
#include <VulkanDevice.hpp>
#include <VulkanSwapChain.hpp>

namespace Jkr {
class Window_base : public SDLWindow {
    protected:
    static const int mMaxFramesInFlight = 2;

    public:
    using SwapChainVulkanImages = std::vector<ksai::VulkanImageExternalHandled>;
    using FrameBufferType       = VulkanFrameBuffer;
    enum ParameterContext : int { None = -3, UI = -1, Background = -1 };

    Window_base()                                    = default;
    Window_base(const Window_base &other)            = delete;
    Window_base &operator=(const Window_base &other) = delete;
    Window_base(Window_base &&other)                 = default;

    virtual const std::array<VulkanCommandBuffer, mMaxFramesInFlight> &
    GetCommandBuffers(ParameterContext inParameter) const {
        return mCommandBuffers;
    }
    const VulkanCommandPool &GetCommandPool() const { return mCommandPool; }
    GETTER &GetCurrentFrame() const { return mCurrentFrame; } // TODO Remove this reference
    GETTER &GetInstance() const { return *mInstance; }
    GETTER &GetRenderPass() const { return mRenderPass; }
    GETTER &GetSwapChainImages() const { return mSwapChainImages; }
    void SetScissor(
         int inX, int inY, int inW, int inH, ParameterContext inContext = ParameterContext::UI);
    void SetDefaultScissor(ParameterContext inContext = ParameterContext::UI);
    void SetViewport(int inX,
                     int inY,
                     int inW,
                     int inH,
                     float inMind,
                     float inMaxD,
                     ParameterContext inContext);
    void SetDefaultViewport(ParameterContext inContext);

    Window_base(const Instance &inInstance,
                std::string_view inTitle,
                int inHeight,
                int inWidth,
                int inOffscreenFrameHeight,
                int inOffscreenFrameWidth);
    Window_base(const Instance &inInstance, int inOffscreenFrameHeight, int inOffscreenFrameWidth);
    ~Window_base() { mInstance->GetDevice().Wait(); }

    protected:
    uint32_t mCurrentFrame       = 0;
    uint32_t mAcquiredImageIndex = 0;
    const Instance *mInstance;
    VulkanSurface mSurface;
    VulkanSwapChain mSwapChain;
    VulkanImage mColorImageRenderTarget;
    VulkanImage mDepthImage;
    VulkanRenderPass<RenderPassContext::MSAA> mRenderPass;
    SwapChainVulkanImages mSwapChainImages;
    std::vector<VulkanImageVMA> mOffscreenImages;
    std::vector<up<FrameBufferType>> mFrameBuffers;
    std::array<VulkanSemaphore, mMaxFramesInFlight> mImageAvailableSemaphores;
    std::array<VulkanSemaphore, mMaxFramesInFlight> mRenderFinishedSemaphores;
    std::array<VulkanFence, mMaxFramesInFlight> mFences;
    VulkanCommandPool mCommandPool;
    std::array<VulkanCommandBuffer, mMaxFramesInFlight> mCommandBuffers;
    glm::uvec2 mOffscreenFrameSize = {1920, 1080};
};

} // namespace Jkr