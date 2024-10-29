#pragma once
#include "PainterParameter.hpp"
#include "SDLWindow.hpp"
#include <VulkanCommandBuffer.hpp>
#include <VulkanDevice.hpp>
#include <VulkanSwapChain.hpp>

namespace Jkr {
///
///@warning IDK Why this thing doesn't work inside of Class with ANDROID NDK, (fails to link)
///
static const int mMaxFramesInFlight = 2;

class Window_base : public SDLWindow {
    public:
    using SwapChainVulkanImages = std::vector<ksai::VulkanImageExternalHandled>;
    using FrameBufferType       = VulkanFrameBuffer;
    enum ParameterContext : int { None = -3, UI = -1, Background = -1 };

    Window_base()                              = default;
    Window_base(Window_base &other)            = delete;
    Window_base &operator=(Window_base &other) = delete;
    Window_base(Window_base &&other)           = default;

    virtual std::array<VulkanCommandBuffer, mMaxFramesInFlight> &
    GetCommandBuffers(ParameterContext inParameter) {
        return mCommandBuffers;
    }
    VulkanCommandPool &GetCommandPool() { return mCommandPool; }
    GETTER GetOffscreenFrameDimension() {
        return glm::vec2(mOffscreenFrameSize.x, mOffscreenFrameSize.y);
    }
    GETTER &GetCurrentFrame() { return mCurrentFrame; } // TODO Remove this reference
    GETTER &GetInstance() { return *mInstance; }
    GETTER &GetRenderPass() { return mRenderPass; }
    GETTER &GetSwapChainImages() { return mSwapChainImages; }
    GETTER &GetSurface() { return mSurface; }
    ///@todo Rename this
    ///@warning For offscreen window (headless), there is only one offscreen image, since
    /// presentation is not being done
    GETTER &GetColorImageRenderTarget() { return mOffscreenImages[0]; }
    GETTER &GetDepthImage() { return mDepthImage; }

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

    Window_base(Instance &inInstance,
                std::string_view inTitle,
                int inHeight,
                int inWidth,
                int inOffscreenFrameHeight,
                int inOffscreenFrameWidth);
    Window_base(Instance &inInstance, int inOffscreenFrameHeight, int inOffscreenFrameWidth);
    ~Window_base() { mInstance->GetDevice().Wait(); }

    protected:
    uint32_t mCurrentFrame       = 0;
    uint32_t mAcquiredImageIndex = 0;
    Instance *mInstance;
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