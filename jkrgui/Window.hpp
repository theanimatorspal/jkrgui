#pragma once
#include "PainterParameter.hpp"
#include "SDLWindow.hpp"
#include <VulkanCommandBuffer.hpp>
#include <VulkanDevice.hpp>
#include <VulkanSwapChain.hpp>

namespace Jkr {
class Window : public SDLWindow {
    protected:
    static const int mMaxFramesInFlight = 2;

    public:
    using SwapChainVulkanImages = std::vector<ksai::VulkanImageExternalHandled>;
    using FrameBufferType =
         VulkanFrameBuffer<3, VulkanImage, VulkanImage, VulkanImageExternalHandled>;
    enum ParameterContext : int { None = -3, UI = -1, Background = -1 };

    public:
    virtual const std::array<VulkanCommandBuffer, mMaxFramesInFlight>&
    GetCommandBuffers(ParameterContext inParameter) const {
        return mCommandBuffers;
    }
    const VulkanCommandPool& GetCommandPool() const { return mCommandPool; }
    GETTER& GetCurrentFrame() const { return mCurrentFrame; } // TODO Remove this reference
    GETTER& GetInstance() const { return mInstance; }
    GETTER& GetRenderPass() const { return mRenderPass; }
    GETTER& GetSwapChainImages() const { return mSwapChainImages; }
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
    // TODO Remove this
    void CmdCopySwapChainImageToBufferPostRendering(VulkanBufferBase& inbuffer);

    Window(const Instance& inInstance, std::string_view inTitle, int inHeight, int inWidth);
    ~Window() { mInstance.GetDevice().Wait(); }

    protected:
    uint32_t mCurrentFrame       = 0;
    uint32_t mAcquiredImageIndex = 0;
    const Instance& mInstance;
    VulkanSurface mSurface;
    VulkanSwapChain mSwapChain;
    VulkanImage mColorImageRenderTarget;
    VulkanImage mDepthImage;
    VulkanRenderPass<RenderPassContext::MSAA> mRenderPass;
    SwapChainVulkanImages mSwapChainImages;
    std::vector<up<FrameBufferType>> mFrameBuffers;
    const std::array<VulkanSemaphore, mMaxFramesInFlight> mImageAvailableSemaphores;
    const std::array<VulkanSemaphore, mMaxFramesInFlight> mRenderFinishedSemaphores;
    std::array<VulkanFence, mMaxFramesInFlight> mFences;
    const VulkanCommandPool mCommandPool;
    const std::array<VulkanCommandBuffer, mMaxFramesInFlight> mCommandBuffers;
};

} // namespace Jkr