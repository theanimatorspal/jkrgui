#include "WindowMulT.hpp"

void Jkr::WindowMulT::Draw(float r, float g, float b, float a, float d)
{
    mUpdateFunction(mData);
    std::array<float, 5>
        ClearValues = { r, g, b, a, d };
    mFences[mCurrentFrame].Wait();
    std::pair<uint32_t, uint32_t> SwapChainResult = mSwapChain.AcquireNextImage(mImageAvailableSemaphores[mCurrentFrame]);
    mFences[mCurrentFrame].Reset();
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
	   {
            mSecondaryCommandBufferBackground[mCurrentFrame].Begin<VulkanCommandBuffer::BeginContext::ContinueRenderPass>(
                mRenderPass,
                0,
                mFrameBuffers[mCurrentFrame]);
            mSecondaryCommandBufferBackground[mCurrentFrame].SetViewport(mSurface);
            mSecondaryCommandBufferBackground[mCurrentFrame].SetScissor(mSurface);
            {
                mBackgroundCallback(mData);
            }
            mSecondaryCommandBufferBackground[mCurrentFrame].End();
        }

	   /* Secondary Command Buffer UI */
	   {
            mSecondaryCommandBuffersUI[mCurrentFrame].Begin<VulkanCommandBuffer::BeginContext::ContinueRenderPass>(
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

        mCommandBuffers[mCurrentFrame].ExecuteCommands(mSecondaryCommandBufferBackground[mCurrentFrame]);
        mCommandBuffers[mCurrentFrame].ExecuteCommands(mSecondaryCommandBuffersUI[mCurrentFrame]);
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
