#include "WindowMulT.hpp"
#include <Vendor/Tracy/tracy/Tracy.hpp>

using namespace Jkr;

Jkr::WindowMulT::WindowMulT(const Instance& inInstance,
                            const sv inTitle,
                            ui inHeight,
                            ui inWidth,
                            ui inNumThreads,
                            std::span<ui> inPerThreadBuffers, // TODO remove this
                            optref<ksai::ThreadPool> inPool)
    : Window(inInstance, inTitle, inHeight, inWidth), mThreadPool(inPool),
      mBackgroundCommandPool(inInstance.GetDevice(), inInstance.GetQueueContext()),
      mUICommandPool(inInstance.GetDevice(), inInstance.GetQueueContext()),
      mSecondaryCommandBuffersBackground{VulkanCommandBuffer(inInstance.GetDevice(), mBackgroundCommandPool, VulkanCommandBuffer::Type::Secondary),
                                         VulkanCommandBuffer(inInstance.GetDevice(), mBackgroundCommandPool, VulkanCommandBuffer::Type::Secondary)},
      mSecondaryCommandBuffersUI{VulkanCommandBuffer(inInstance.GetDevice(), mUICommandPool, VulkanCommandBuffer::Type::Secondary),
                                 VulkanCommandBuffer(inInstance.GetDevice(), mUICommandPool, VulkanCommandBuffer::Type::Secondary)} {
          for (int i = 0; i < inNumThreads; i++) {
                    mThreadCommandBuffers.emplace_back(mu<ThreadCommandBufferArray>(inInstance));
          }
}

void Jkr::WindowMulT::Draw(float r, float g, float b, float a, float d) {
          mFences[mCurrentFrame].Wait();
          mFences[mCurrentFrame].Reset();
          ZoneScoped;
          mUpdateFunction(mData);

          std::array<float, 5> ClearValues = {r, g, b, a, d};
          std::pair<uint32_t, uint32_t> SwapChainResult = mSwapChain.AcquireNextImage(mImageAvailableSemaphores[mCurrentFrame]);
          mAcquiredImageIndex = SwapChainResult.second;
          if (mThreadPool.has_value()) {
                    auto& tp = mThreadPool.value().get();
                    tp.Add_Job([this]() { Jkr::WindowMulT::CmdBackground(); });
                    CmdUI();
          } else {
                    CmdBackground();
                    CmdUI();
          }

          mCommandBuffers[mCurrentFrame].Reset();
          mCommandBuffers[mCurrentFrame].Begin();

          mComputeDispatchFunction(mData);

          mCommandBuffers[mCurrentFrame].BeginRenderPass<VulkanCommandBuffer::RenderPassBeginContext::SecondaryCommandBuffers>(
           mRenderPass, mSurface, *mFrameBuffers[mAcquiredImageIndex], ClearValues);

          /* Secondary Command Buffer Background */
          if (mThreadPool.has_value()) {
                    mThreadPool.value().get().Wait();
                    mThreadPool.value().get().Wait();
          }

          mCommandBuffers[mCurrentFrame].ExecuteCommands(mSecondaryCommandBuffersUI[mCurrentFrame]);
          mCommandBuffers[mCurrentFrame].ExecuteCommands(mSecondaryCommandBuffersBackground[mCurrentFrame]);
          mCommandBuffers[mCurrentFrame].EndRenderPass();

          mPostRenderingFunction(mData);
          mCommandBuffers[mCurrentFrame].End();

          mInstance.GetGraphicsQueue().Submit<SubmitContext::ColorAttachment>(mImageAvailableSemaphores[mCurrentFrame],
                                                                              mRenderFinishedSemaphores[mCurrentFrame],
                                                                              mFences[mCurrentFrame],
                                                                              mCommandBuffers[mCurrentFrame]);
          uint32_t Result = mInstance.GetGraphicsQueue().Present<mMaxFramesInFlight, SubmitContext::ColorAttachment>(
           mSwapChain, mRenderFinishedSemaphores[mCurrentFrame], mAcquiredImageIndex);

          if (mSwapChain.ImageIsNotOptimal(Result)) {
                    Refresh();
          } else {
                    mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;
          }
}

void Jkr::WindowMulT::CmdBackground() {
          ZoneScoped;
          mSecondaryCommandBuffersBackground[mCurrentFrame].Begin<VulkanCommandBuffer::BeginContext::ContinueRenderPassAndOneTimeSubmit>(
           mRenderPass, 0, *mFrameBuffers[mAcquiredImageIndex]);
          mSecondaryCommandBuffersBackground[mCurrentFrame].SetViewport(mSurface);
          mSecondaryCommandBuffersBackground[mCurrentFrame].SetScissor(mSurface);
          {
                    ZoneScoped;
                    mBackgroundCallback(mData);
          }
          //    TracyVkZone(mctx_bg1, mSecondaryCommandBuffersBackground[mCurrentFrame].GetCommandBufferHandle(), "BG1");
          mSecondaryCommandBuffersBackground[mCurrentFrame].End();
}

void Jkr::WindowMulT::CmdUI() {
          ZoneScoped;
          mSecondaryCommandBuffersUI[mCurrentFrame].Begin<VulkanCommandBuffer::BeginContext::ContinueRenderPassAndOneTimeSubmit>(
           mRenderPass, 0, *mFrameBuffers[mAcquiredImageIndex]);

          mSecondaryCommandBuffersUI[mCurrentFrame].SetViewport(mSurface);
          mSecondaryCommandBuffersUI[mCurrentFrame].SetScissor(mSurface);
          {
                    ZoneScoped;
                    mDrawFunction(mData);
          }
          mSecondaryCommandBuffersUI[mCurrentFrame].End();
}

void Jkr::WindowMulT::Refresh() {
          ZoneScoped;
          mSurface.ProcessCurrentSurfaceConditions(mInstance.GetPhysicalDevice());
          mSwapChain = VulkanSwapChain<mMaxFramesInFlight>(mInstance.GetDevice(), mInstance.GetQueueContext(), mSurface, mSwapChain);
          mSwapChainImages = mSwapChain.GetVulkanImages(mInstance.GetDevice(), mSurface);
          mColorImageRenderTarget = VulkanImage<ImageContext::ColorAttach>(mInstance.GetDevice(), mSurface, 4);
          mDepthImage = VulkanImage<ImageContext::DepthImage>(mInstance.GetDevice(), mSurface, 4);

          for (int i = 0; i < mSwapChainImages.size(); i++) {
                    mFrameBuffers[i] =
                     MakeUp<FrameBufferType>(mInstance.GetDevice(), mRenderPass, mColorImageRenderTarget, mDepthImage, mSwapChainImages[i]);
          }
          mResizeFunction(nullptr);
}
