#include "Window.hpp"
#include <Vendor/Tracy/tracy/TracyVulkan.hpp>
#include <queue>
#include <thread>

namespace Jkr {

struct CommandBuffers2 {
    std::array<VulkanCommandBuffer, 2> mCmdBuffers;
    CommandBuffers2(const VulkanDevice& inDev, const VulkanCommandPool& inPool)
        : mCmdBuffers {
            VulkanCommandBuffer(inDev, inPool, VulkanCommandBuffer::Type::Secondary),
            VulkanCommandBuffer(inDev, inPool, VulkanCommandBuffer::Type::Secondary)
        }
    {
    }
};

struct ThreadData {
    ThreadData(const Instance& inInstance, ui inNumCmdBuffers)
        : mCommandPool(inInstance.GetDevice(), inInstance.GetQueueContext())
    {
        for (int i = 0; i < inNumCmdBuffers; i++) {
            mCommandBuffers.emplace_back(mu<CommandBuffers2>(inInstance.GetDevice(), mCommandPool));
        }
    }

    VulkanCommandPool mCommandPool;
    v<up<CommandBuffers2>> mCommandBuffers;
};

class WindowMulT : public Window {
public:
    const VulkanCommandBuffer& GetSecondaryCmdBufferBackground() const override { return mSecondaryCommandBuffersBackground[mCurrentFrame]; }
    const VulkanCommandBuffer& GetSecondaryCmdBufferUI() const override { return mSecondaryCommandBuffersUI[mCurrentFrame]; }
    const VulkanCommandBuffer& GetUtilCommandBuffer() const override { return mCommandBuffers[mCurrentFrame]; }
    const std::array<VulkanCommandBuffer, 2U>& GetCommandBuffers(ParameterContext inParameter, int inThreadId = 0, int inObjId = 0) const override
    {
        switch (inParameter) {
        case Jkr::Window::None:
            return mCommandBuffers;
        case Jkr::Window::UI:
            return mSecondaryCommandBuffersUI;
        case Jkr::Window::Background:
            return mSecondaryCommandBuffersBackground;
        default:
            return mThreadData[inThreadId]->mCommandBuffers[inObjId]->mCmdBuffers;
        }
    }
    WindowMulT(const Instance& inInstance, const sv inTitle, ui inHeight, ui inWidth, ui inNumThreads, std::span<ui> inPerThreadBuffers, optref<ksai::ThreadPool> inPool = std::nullopt)
        : Window(inInstance, inTitle, inHeight, inWidth)
        , mThreadPool(inPool)
        , mBackgroundCommandPool(inInstance.GetDevice(), inInstance.GetQueueContext())
        , mUICommandPool(inInstance.GetDevice(), inInstance.GetQueueContext())
        , mSecondaryCommandBuffersBackground {
            VulkanCommandBuffer(inInstance.GetDevice(), mBackgroundCommandPool, VulkanCommandBuffer::Type::Secondary),
            VulkanCommandBuffer(inInstance.GetDevice(), mBackgroundCommandPool, VulkanCommandBuffer::Type::Secondary)
        }
        , mSecondaryCommandBuffersUI { VulkanCommandBuffer(inInstance.GetDevice(), mUICommandPool, VulkanCommandBuffer::Type::Secondary), VulkanCommandBuffer(inInstance.GetDevice(), mUICommandPool, VulkanCommandBuffer::Type::Secondary) }
    {
        for (int i = 0; i < inNumThreads; i++) {
            mThreadData.emplace_back(MakeUp<ThreadData>(mInstance, inPerThreadBuffers[i]));
        }
    }

    ~WindowMulT()
    {
    }
    virtual void Refresh() override;
    virtual void Draw(float r = 0.1f, float g = 0.1f, float b = 0.1f, float a = 0.1f, float d = 1.0f) override;
    void BeginUpdate();
    void BeginDispatch();
    void BeginDraw(float r = 0.1f, float g = 0.1f, float b = 0.1f, float a = 0.1f, float d = 1.0f);
    void EndDraw();
    void EndDispatch();
    void EndUpdate();
    void BeginUI();
    void EndUI();

private:
    optref<ksai::ThreadPool> mThreadPool;
    VulkanCommandPool mBackgroundCommandPool;
    VulkanCommandPool mUICommandPool;
    std::array<VulkanCommandBuffer, mMaxFramesInFlight> mSecondaryCommandBuffersBackground;
    std::array<VulkanCommandBuffer, mMaxFramesInFlight> mSecondaryCommandBuffersUI;
    v<up<VulkanCommandBuffer>> mSecondaryCommandBuffers;
    v<up<ThreadData>> mThreadData;

    void CmdBackground();
    void CmdUI();
public:
    void SetBackgroundCallBack(std::function<void(void*)> inFunction) { mBackgroundCallback = inFunction; }
    void SetDedicated3DCallback(std::function<void(void*)> inFunction) { mDedicated3DCallback = inFunction; }

private:
    std::function<void(void*)> mBackgroundCallback = [](void*) {};
    std::function<void(void*)> mDedicated3DCallback = [](void*) {};
};

} // namespace Jkr

inline void Jkr::WindowMulT::BeginUpdate()
{
    mFences[mCurrentFrame].Wait();
    mFences[mCurrentFrame].Reset();
}

inline void Jkr::WindowMulT::BeginDispatch()
{
    std::pair<uint32_t, uint32_t> SwapChainResult = mSwapChain.AcquireNextImage(mImageAvailableSemaphores[mCurrentFrame]);
    mAcquiredImageIndex = SwapChainResult.second;
    mCommandBuffers[mCurrentFrame].Reset();
    mCommandBuffers[mCurrentFrame].Begin();
}

inline void Jkr::WindowMulT::BeginDraw(float r, float g, float b, float a, float d)
{
    std::array<float, 5>
        ClearValues = { r, g, b, a, d };
    mCommandBuffers[mCurrentFrame].BeginRenderPass<VulkanCommandBuffer::RenderPassBeginContext::SecondaryCommandBuffers>(
        mRenderPass,
        mSurface,
        *mFrameBuffers[mAcquiredImageIndex], // यो स्थानमा जहिल्यै झुक्किन्छ । फ्रेम बफर एउटा हुने हो ।  "Acquired Image Index" अनुसार ।
        ClearValues);
}

inline void Jkr::WindowMulT::EndDraw()
{
    mCommandBuffers[mCurrentFrame].ExecuteCommands(mSecondaryCommandBuffersUI[mCurrentFrame]);
    mCommandBuffers[mCurrentFrame].EndRenderPass();
}

inline void Jkr::WindowMulT::EndDispatch()
{
    mCommandBuffers[mCurrentFrame].End();
}

inline void Jkr::WindowMulT::EndUpdate()
{
    mInstance.GetGraphicsQueue().Submit<SubmitContext::ColorAttachment>(
        mImageAvailableSemaphores[mCurrentFrame],
        mRenderFinishedSemaphores[mCurrentFrame],
        mFences[mCurrentFrame],
        mCommandBuffers[mCurrentFrame]);
    uint32_t Result = mInstance.GetGraphicsQueue().Present<mMaxFramesInFlight, SubmitContext::ColorAttachment>(
        mSwapChain,
        mRenderFinishedSemaphores[mCurrentFrame],
        mAcquiredImageIndex);

    if (mSwapChain.ImageIsNotOptimal(Result)) {
        Refresh();
    } else {
        mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;
    }
}

inline void Jkr::WindowMulT::BeginUI()
{
    mSecondaryCommandBuffersUI[mCurrentFrame].Begin<VulkanCommandBuffer::BeginContext::ContinueRenderPassAndOneTimeSubmit>(
        mRenderPass,
        0,
        *mFrameBuffers[mAcquiredImageIndex]);

    mSecondaryCommandBuffersUI[mCurrentFrame].SetViewport(mSurface);
    mSecondaryCommandBuffersUI[mCurrentFrame].SetScissor(mSurface);
}

inline void Jkr::WindowMulT::EndUI()
{
    mSecondaryCommandBuffersUI[mCurrentFrame].End();
}
