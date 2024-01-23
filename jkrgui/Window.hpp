#pragma once
#include "SDLWindow.hpp"

namespace Jkr {
class Window : public SDLWindow {
public:
    using SwapChainVulkanImages = std::vector<VulkanImage<ImageContext::ExternalHandled>>;
    using FrameBufferType = VulkanFrameBuffer<
        RenderPassContext::Default, 2, VulkanImage<ImageContext::ExternalHandled>,
        VulkanImage<ImageContext::DepthImage>>;

    enum ParameterContext : int {
        None = -3,
        UI = -2,
        Background = -1,
    };

public:
    /* Virtual because WindowMULT uses these [Backwards Compatibility] */
    const VulkanCommandPool& GetCommandPool() const { return mCommandPool; }
    virtual const std::array<VulkanCommandBuffer, 2U>& GetCommandBuffers(ParameterContext inParameter, int inP1 = 0, int inP2 = 0) const { return mCommandBuffers; }
    virtual const VulkanCommandBuffer& GetUtilCommandBuffer() const { return mCommandBuffers[mCurrentFrame]; }
    virtual const VulkanCommandBuffer& GetSecondaryCmdBufferUI() const { return mCommandBuffers[mCurrentFrame]; }
    virtual const VulkanCommandBuffer& GetSecondaryCmdBufferBackground() const { return mCommandBuffers[mCurrentFrame]; }
    virtual const VulkanCommandBuffer& GetSecondaryCmdBufferInThread(ui inThreadID, ui inObjId) const { return mCommandBuffers[mCurrentFrame]; }
    GETTER& GetCurrentFrame() const { return mCurrentFrame; }
    GETTER& GetInstance() const { return mInstance; }
    GETTER& GetRenderPass() const { return mRenderPass; }
    GETTER& GetSwapChainImages() const { return mSwapChainImages; }

    SETTER SetDrawCallBack(std::function<void(void*)> inFunction) { mDrawFunction = inFunction; }
    SETTER SetUpdateCallBack(std::function<void(void*)> inFunction) { mUpdateFunction = inFunction; }
    SETTER SetComputeDispatchCallBack(std::function<void(void*)> inFunction) { mComputeDispatchFunction = inFunction; }
    SETTER SetResizeCallBack(std::function<void(void*)> inFunction) { mResizeFunction = inFunction; }
    SETTER SetContextData(void* inData) { mData = inData; }
    SETTER SetScissor(const vk::ArrayProxy<vk::Rect2D>& inScissor, ParameterContext inContext = ParameterContext::UI);
    SETTER ResetScissor(ParameterContext inContext = ParameterContext::UI);

    Window(const Instance& inInstance, std::string_view inTitle, int inHeight, int inWidth);
    ~Window() { mInstance.GetDevice().Wait(); }
    virtual void Refresh();
    virtual void Draw(float r = 0.1f, float g = 0.1f, float b = 0.1f, float a = 0.1f, float d = 1.0f);

protected:
    std::function<void(void*)> mDrawFunction = [](void*) {};
    std::function<void(void*)> mUpdateFunction = [](void*) {};
    std::function<void(void*)> mComputeDispatchFunction = [](void*) {};
    std::function<void(void*)> mResizeFunction = [](void*) {};

protected:
    static const int mMaxFramesInFlight = 2;
    uint32_t mCurrentFrame = 0;
    uint32_t mAcquiredImageIndex = 0;
    const Instance& mInstance;
    VulkanSurface mSurface;
    VulkanSwapChain<mMaxFramesInFlight> mSwapChain;
    VulkanImage<ImageContext::DepthImage> mDepthImage;
    VulkanRenderPass<RenderPassContext::Default> mRenderPass;
    SwapChainVulkanImages mSwapChainImages;
    std::vector<up<FrameBufferType>> mFrameBuffers;
    const std::array<VulkanSemaphore, mMaxFramesInFlight> mImageAvailableSemaphores;
    const std::array<VulkanSemaphore, mMaxFramesInFlight> mRenderFinishedSemaphores;
    std::array<VulkanFence, mMaxFramesInFlight> mFences;
    const VulkanCommandPool mCommandPool;
    const std::array<VulkanCommandBuffer, mMaxFramesInFlight> mCommandBuffers;
};

}

inline void Jkr::Window::SetScissor(const vk::ArrayProxy<vk::Rect2D>& inScissor, ParameterContext inContext)
{
    this->GetCommandBuffers(inContext)[mCurrentFrame].GetCommandBufferHandle().setScissor(0, inScissor);
}
inline void Jkr::Window::ResetScissor(ParameterContext inContext)
{
    vk::Rect2D Rect(vk::Offset2D(0), mDepthImage.GetImageExtent());
    this->GetCommandBuffers(inContext)[mCurrentFrame].GetCommandBufferHandle().setScissor(0, Rect);
}
