#include "WindowMulT.hpp"
#include "Offscreen.hpp"
#include "Renderers/Renderer_base.hpp"
#include "VulkanCommandBuffer.hpp"
#include "vulkan/vulkan_enums.hpp"
#include <Vendor/Tracy/tracy/Tracy.hpp>

using namespace Jkr;

Jkr::WindowMulT::WindowMulT(const Instance& inInstance,
                            const sv inTitle,
                            ui inHeight,
                            ui inWidth,
                            ui inNumThreads,
                            std::span<ui> inPerThreadBuffers, // TODO remove this
                            optref<ksai::ThreadPool> inPool)
    : Window(inInstance, inTitle, inHeight, inWidth),
      mThreadPool(inPool),
      mUICommandPool(inInstance.GetDevice(), inInstance.GetQueueContext()),
      mSecondaryCommandBuffersUI{
           VulkanCommandBuffer(
                inInstance.GetDevice(), mUICommandPool, VulkanCommandBuffer::Type::Secondary),
           VulkanCommandBuffer(
                inInstance.GetDevice(), mUICommandPool, VulkanCommandBuffer::Type::Secondary)}

{
    for (int i = 0; i < inNumThreads; i++) {
        mThreadCommandBuffers.emplace_back(mu<ThreadCommandBufferArray>(inInstance));
    }
}

void Jkr::WindowMulT::Refresh() {
    ZoneScoped;
    mSurface.ProcessCurrentSurfaceConditions(mInstance.GetPhysicalDevice());
    mSwapChain =
         VulkanSwapChain(mInstance.GetDevice(), mInstance.GetQueueContext(), mSurface, mSwapChain);
    mSwapChainImages = mSwapChain.GetVulkanImages(mInstance.GetDevice(), mSurface);
    mColorImageRenderTarget =
         VulkanImage<ImageContext::ColorAttach>(mInstance.GetDevice(), mSurface, 4);
    mDepthImage = VulkanImage<ImageContext::DepthImage>(mInstance.GetDevice(), mSurface, 4);

    for (int i = 0; i < mSwapChainImages.size(); i++) {
        mFrameBuffers[i] = MakeUp<FrameBufferType>(mInstance.GetDevice(),
                                                   mRenderPass,
                                                   mColorImageRenderTarget,
                                                   mDepthImage,
                                                   mSwapChainImages[i]);
    }

    if (mShadowPass) {
        // TODO Recreate Shadow pass
    }

    mResizeFunction(nullptr);
}

namespace Jkr {
const std::array<VulkanCommandBuffer, 2U>&
Jkr::WindowMulT::GetCommandBuffers(ParameterContext inParameter) const {
    switch (inParameter) {
        case Jkr::Window::None:
            return mCommandBuffers;
        case Jkr::Window::UI:
            return mSecondaryCommandBuffersUI;
        default:
            return mThreadCommandBuffers[inParameter]->mCommandBuffers;
    }
}

void Jkr::WindowMulT::BeginUpdates() {
    mFences[mCurrentFrame].Wait();
    mFences[mCurrentFrame].Reset();
}

void WindowMulT::EndUpdates() {
    std::pair<uint32_t, uint32_t> SwapChainResult =
         mSwapChain.AcquireNextImage(mImageAvailableSemaphores[mCurrentFrame]);
    mAcquiredImageIndex = SwapChainResult.second;
}

void WindowMulT::BeginDispatches() {
    mCommandBuffers[mCurrentFrame].Reset();
    mCommandBuffers[mCurrentFrame].Begin();
}

void WindowMulT::EndDispatches() {}

void WindowMulT::BeginDraws(
     float r = 0.1f, float g = 0.1f, float b = 0.1f, float a = 0.1f, float d = 1.0f) {
    std::array<float, 5> ClearValues = {r, g, b, a, d};
    mCommandBuffers[mCurrentFrame]
         .BeginRenderPass<VulkanCommandBuffer::RenderPassBeginContext::SecondaryCommandBuffers>(
              mRenderPass, mSurface.GetExtent(), *mFrameBuffers[mAcquiredImageIndex], ClearValues);
}

void WindowMulT::EndDraws() { mCommandBuffers[mCurrentFrame].EndRenderPass(); }

void WindowMulT::Present() {
    mCommandBuffers[mCurrentFrame].End();

    mInstance.GetGraphicsQueue().Submit<SubmitContext::ColorAttachment>(
         mImageAvailableSemaphores[mCurrentFrame],
         mRenderFinishedSemaphores[mCurrentFrame],
         mFences[mCurrentFrame],
         mCommandBuffers[mCurrentFrame]);
    uint32_t Result = mInstance.GetGraphicsQueue().Present<SubmitContext::ColorAttachment>(
         mSwapChain, mRenderFinishedSemaphores[mCurrentFrame], mAcquiredImageIndex);

    if (mSwapChain.ImageIsNotOptimal(Result)) {
        Refresh();
    } else {
        mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;
    }
}

void WindowMulT::BeginUIs() {
    mSecondaryCommandBuffersUI[mCurrentFrame]
         .Begin<VulkanCommandBuffer::BeginContext::ContinueRenderPassAndOneTimeSubmit>(
              mRenderPass, 0, *mFrameBuffers[mAcquiredImageIndex]);

    // TODO Replace this with Our Window Functions
    mSecondaryCommandBuffersUI[mCurrentFrame].SetViewport(mSurface);
    mSecondaryCommandBuffersUI[mCurrentFrame].SetScissor(mSurface);
}

void WindowMulT::EndUIs() { mSecondaryCommandBuffersUI[mCurrentFrame].End(); }

void WindowMulT::ExecuteUIs() {
    mCommandBuffers[mCurrentFrame].ExecuteCommands(mSecondaryCommandBuffersUI[mCurrentFrame]);
}

using namespace Jkr;
void WindowMulT::BeginShadowPass(float ind) {
    mCommandBuffers[mCurrentFrame].BeginRenderPass(
         mShadowPass->GetRenderPass(),
         vk::Extent2D(mShadowPassSize.x, mShadowPassSize.y),
         mShadowPass->GetFrameBuffer(),
         {1.0f, 1.0f, 1.0f, 1.0f, ind});
}
void WindowMulT::EndShadowPass() {

    mCommandBuffers[mCurrentFrame].EndRenderPass();
    mShadowPass->GetDepthImagePainterParameter().GetDepthImage().CmdTransitionImageLayout(
         mCommandBuffers[mCurrentFrame],
         vk::ImageLayout::eDepthStencilReadOnlyOptimal,
         vk::ImageLayout::eGeneral,
         vk::PipelineStageFlagBits::eLateFragmentTests,
         vk::PipelineStageFlagBits::eFragmentShader,
         vk::AccessFlagBits::eDepthStencilAttachmentWrite,
         vk::AccessFlagBits::eShaderRead);
}

void WindowMulT::ExecuteThreadCommandBuffer(int inThreadId) {
    auto& cmd = GetCommandBuffers((Renderer::CmdParam)inThreadId)[mCurrentFrame];
    mCommandBuffers[mCurrentFrame].ExecuteCommands(cmd);
}

void WindowMulT::BeginThreadCommandBuffer(int inThreadId) {
    mThreadCommandBuffers[inThreadId]->mCommandBuffers[mCurrentFrame].Reset();
    mThreadCommandBuffers[inThreadId]
         ->mCommandBuffers[mCurrentFrame]
         .Begin<VulkanCommandBuffer::BeginContext::ContinueRenderPassAndOneTimeSubmit>(
              mRenderPass, 0, *mFrameBuffers[mAcquiredImageIndex]);
}

void WindowMulT::EndThreadCommandBuffer(int inThreadId) {
    mThreadCommandBuffers[inThreadId]->mCommandBuffers[mCurrentFrame].End();
}

void WindowMulT::BuildShadowPass() {
    mShadowPass = mu<ShadowPass>(
         mInstance, mDepthImage.GetImageExtent().width, mDepthImage.GetImageExtent().height);
    mShadowPassSize =
         glm::uvec2(mDepthImage.GetImageExtent().width, mDepthImage.GetImageExtent().height);
}

} // namespace Jkr