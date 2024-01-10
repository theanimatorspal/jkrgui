#pragma once
#define SDL_MAIN_HANDLED
#include "Instance.hpp"
#include <any>
#include <functional>

namespace Jkr {
class SDLWindow {
public:
    SDLWindow(std::string_view inName, int inHeight, int inWidth);
    ~SDLWindow();
    void SetSize(int inWidth, int inHeight);
    void SetTitle(std::string inString) const;
    void SetWindowBorderless();
    std::pair<int, int> GetWindowSize() const;
    GETTER GetWindowHandle() const { return mSDLWindowPtr; }
    void SetWindowShouldClose(bool inValue) { mWindowShouldClose = inValue; }
    void SetResizeCallBack(const std::function<void(void*)>& inFunction) { mResizeFunction = inFunction; }
    void ToggleWindowFullScreen();
    void Minimize();
    static auto GetWindowCurrentTime() { return SDL_GetTicks(); }

protected:
    void* mData = nullptr;
    std::function<void(void*)> mResizeFunction = [](void*) {};
    bool mWindowShouldClose = false;
    bool mWindowIsFullScreen = false;
    std::string mName;
    int mHeight;
    int mWidth;
    SDL_Window* mSDLWindowPtr;
};
}

namespace Jkr {
class Window : public SDLWindow {
public:
    const VulkanCommandPool& GetCommandPool() const { return mCommandPool; }
    virtual const std::array<VulkanCommandBuffer, 2U>& GetCommandBuffers() const { return mCommandBuffers; }
    virtual const VulkanCommandBuffer& GetUtilCommandBuffer() const { return mCommandBuffers[mCurrentFrame]; }
    virtual const VulkanCommandBuffer& GetSecondaryCmdBufferUI() const { return mCommandBuffers[mCurrentFrame]; }
    virtual const VulkanCommandBuffer& GetSecondaryCmdBufferBackground() const { return mCommandBuffers[mCurrentFrame]; }
    virtual const VulkanCommandBuffer& GetSecondaryCmdBufferInThread(ui inThreadID, ui inObjId) const { return mCommandBuffers[mCurrentFrame]; }

    Window(const Instance& inInstance, std::string_view inTitle, int inHeight, int inWidth);
    ~Window() { mInstance.GetDevice().Wait(); }
    void SetDrawCallBack(const std::function<void(void*)>& inFunction) { mDrawFunction = inFunction; }
    void SetUpdateCallBack(const std::function<void(void*)>& inFunction) { mUpdateFunction = inFunction; }
    void SetComputeDispatchCallBack(const std::function<void(void*)>& inFunction) { mComputeDispatchFunction = inFunction; }
    void SetContextData(void* inData) { mData = inData; }
    void Refresh();
    virtual void Draw(float r = 0.1f, float g = 0.1f, float b = 0.1f, float a = 0.1f, float d = 1.0f);
    void SetScissor(const vk::ArrayProxy<vk::Rect2D>& inScissor)
    {
        this->GetCommandBuffers()[mCurrentFrame].GetCommandBufferHandle().setScissor(0,
            inScissor);
    }
    void ResetScissor()
    {
        vk::Rect2D Rect(vk::Offset2D(0), mDepthImage.GetImageExtent());
        this->GetCommandBuffers()[mCurrentFrame].GetCommandBufferHandle().setScissor(0, Rect);
    }
    GETTER& GetCurrentFrame() const { return mCurrentFrame; }
    GETTER& GetInstance() const { return mInstance; }
    GETTER& GetRenderPass() const { return mRenderPass; }

protected:
    std::function<void(void*)> mDrawFunction = [](void*) {};
    std::function<void(void*)> mUpdateFunction = [](void*) {};
    std::function<void(void*)> mComputeDispatchFunction = [](void*) {};

protected:
    static const int mMaxFramesInFlight = 2;
    using SwapChainVulkanImages = std::vector<VulkanImage<ImageContext::ExternalHandled>>;
    using FrameBufferType = VulkanFrameBuffer<
        RenderPassContext::Default, 2, VulkanImage<ImageContext::ExternalHandled>,
        VulkanImage<ImageContext::DepthImage>>;

protected:
    const Instance& mInstance;
    VulkanSurface mSurface;
    VulkanSwapChain<mMaxFramesInFlight> mSwapChain;
    VulkanImage<ImageContext::DepthImage> mDepthImage;
    VulkanRenderPass<RenderPassContext::Default> mRenderPass;
    SwapChainVulkanImages mSwapChainImages;
    std::array<FrameBufferType, mMaxFramesInFlight> mFrameBuffers;

protected:
    const std::array<VulkanSemaphore, mMaxFramesInFlight> mImageAvailableSemaphores;
    const std::array<VulkanSemaphore, mMaxFramesInFlight> mRenderFinishedSemaphores;
    std::array<VulkanFence, mMaxFramesInFlight> mFences;
    uint32_t mCurrentFrame = 0;
    uint32_t mAcquiredImageIndex = 0;

protected:
    const VulkanCommandPool mCommandPool;
    const std::array<VulkanCommandBuffer, mMaxFramesInFlight> mCommandBuffers;
};
}
