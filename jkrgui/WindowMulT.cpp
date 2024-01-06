#include "WindowMulT.hpp"

void Jkr::WindowMulT::Draw(float r, float g, float b, float a, float d)
{
    mUpdateFunction(mData);
    std::array<float, 5>
        ClearValues = { r, g, b, a, d };
    mFences[mCurrentFrame].Wait();
    std::pair<uint32_t, uint32_t> SwapChainResult = mSwapChain.AcquireNextImage(mImageAvailableSemaphores[mCurrentFrame]);
    mFences[mCurrentFrame].Reset();
    auto& CommandBuffers = this->GetCommandBuffers();
    if (!mSwapChain.ImageIsNotOptimal(SwapChainResult)) {
        mAcquiredImageIndex = SwapChainResult.second;
        CommandBuffers[mCurrentFrame].Reset();
        CommandBuffers[mCurrentFrame].Begin();

        mComputeDispatchFunction(mData);

        CommandBuffers[mCurrentFrame].BeginRenderPass<VulkanCommandBuffer::RenderPassBeginContext::SecondaryCommandBuffers>(
            mRenderPass,
            mSurface,
            mFrameBuffers[mAcquiredImageIndex], // यो स्थानमा जहिल्यै झुक्किन्छ । फ्रेम बफर एउटा हुने हो ।  "Acquired Image Index" अनुसार ।
            ClearValues);

        mDrawFunction(mData);
        CommandBuffers[mCurrentFrame].EndRenderPass();

        Executeall();
        CommandBuffers[mCurrentFrame].End();

        mInstance.GetGraphicsQueue().Submit<SubmitContext::ColorAttachment>(
            mImageAvailableSemaphores[mCurrentFrame],
            mRenderFinishedSemaphores[mCurrentFrame],
            mFences[mCurrentFrame],
            CommandBuffers[mCurrentFrame]);

        ui Result = mInstance.GetGraphicsQueue().Present<mMaxFramesInFlight, SubmitContext::ColorAttachment>(
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
