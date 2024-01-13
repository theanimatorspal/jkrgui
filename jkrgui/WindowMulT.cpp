#include "WindowMulT.hpp"
#include <Vendor/Tracy/tracy/Tracy.hpp>

void Jkr::WindowMulT::Draw(float r, float g, float b, float a, float d)
{
    mUpdateFunction(mData);
    mFences[mCurrentFrame].Wait();
    mFences[mCurrentFrame].Reset();

    if (mThreadPool.has_value()) {
        auto& tp = mThreadPool.value().get();
        tp.Add_Job([this]() { Jkr::WindowMulT::CmdBackground(); });
        tp.Add_Job([this]() { Jkr::WindowMulT::CmdUI(); });
    } else {
        CmdBackground();
        CmdUI();
    }

    std::array<float, 5>
        ClearValues = { r, g, b, a, d };
    std::pair<uint32_t, uint32_t> SwapChainResult = mSwapChain.AcquireNextImage(mImageAvailableSemaphores[mCurrentFrame]);
    if (!mSwapChain.ImageIsNotOptimal(SwapChainResult)) {
        mAcquiredImageIndex = SwapChainResult.second;
        mCommandBuffers[mCurrentFrame].Reset();
        mCommandBuffers[mCurrentFrame].Begin();

        mComputeDispatchFunction(mData);
        mCommandBuffers[mCurrentFrame].BeginRenderPass<VulkanCommandBuffer::RenderPassBeginContext::SecondaryCommandBuffers>(
            mRenderPass,
            mSurface,
            mFrameBuffers[mAcquiredImageIndex], // यो स्थानमा जहिल्यै झुक्किन्छ । फ्रेम बफर एउटा हुने हो ।  "Acquired Image Index" अनुसार ।
            ClearValues);

        /* Secondary Command Buffer Background */
        if (mThreadPool.has_value()) {
            mThreadPool.value().get().Wait();
        }

        mCommandBuffers[mCurrentFrame].ExecuteCommands(mSecondaryCommandBuffersUI[mCurrentFrame]);
        mCommandBuffers[mCurrentFrame].ExecuteCommands(mSecondaryCommandBuffersBackground[mCurrentFrame]);
        mCommandBuffers[mCurrentFrame].EndRenderPass();
        mCommandBuffers[mCurrentFrame].End();

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
    } else {
        Refresh();
    }
}

void Jkr::WindowMulT::CmdBackground()
{
    mSecondaryCommandBuffersBackground[mCurrentFrame].Begin<VulkanCommandBuffer::BeginContext::ContinueRenderPassAndOneTimeSubmit>(
        mRenderPass,
        0,
        mFrameBuffers[mCurrentFrame]);
    mSecondaryCommandBuffersBackground[mCurrentFrame].SetViewport(mSurface);
    mSecondaryCommandBuffersBackground[mCurrentFrame].SetScissor(mSurface);
    {
        mBackgroundCallback(mData);
    }
//    TracyVkZone(mctx_bg1, mSecondaryCommandBuffersBackground[mCurrentFrame].GetCommandBufferHandle(), "BG1");
    mSecondaryCommandBuffersBackground[mCurrentFrame].End();
}

void Jkr::WindowMulT::CmdUI()
{
    mSecondaryCommandBuffersUI[mCurrentFrame].Reset();
    mSecondaryCommandBuffersUI[mCurrentFrame].Begin<VulkanCommandBuffer::BeginContext::ContinueRenderPassAndOneTimeSubmit>(
        mRenderPass,
        0,
        mFrameBuffers[mCurrentFrame]);

    mSecondaryCommandBuffersUI[mCurrentFrame].SetViewport(mSurface);
    mSecondaryCommandBuffersUI[mCurrentFrame].SetScissor(mSurface);
    {
        mDrawFunction(mData);
    }
    mSecondaryCommandBuffersUI[mCurrentFrame].End();
}
