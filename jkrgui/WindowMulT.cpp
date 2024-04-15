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
