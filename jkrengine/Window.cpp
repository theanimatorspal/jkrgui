#include "Window.hpp"
#include "Offscreen.hpp"
#include "Renderers/Renderer_base.hpp"
#include "Renderers/ThreeD/Simple3D.hpp"
#include "Renderers/ThreeD/World3D.hpp"
#include "VulkanCommandBuffer.hpp"
#include "vulkan/vulkan_enums.hpp"
#include <Vendor/Tracy/tracy/Tracy.hpp>

using namespace Jkr;

Window::Window(Instance &inInstance,
               const sv inTitle,
               ui inHeight,
               ui inWidth,
               ui inOffscreenFrameHeight,
               ui inOffscreenFrameWidth,
               ui inNumThreads,
               std::span<ui> inPerThreadBuffers, // TODO remove this
               optref<ksai::ThreadPool> inPool)
    : Window_base(
           inInstance, inTitle, inHeight, inWidth, inOffscreenFrameHeight, inOffscreenFrameWidth),
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

Window::Window(Instance &inInstance,
               std::span<ui> inPerThreadBuffers,
               ui inOffscreenFrameHeight,
               ui inOffscreenFrameWidth,
               ui inNumThreads,
               optref<ksai::ThreadPool> inPool)
    : Window_base(inInstance, inOffscreenFrameHeight, inOffscreenFrameWidth),
      mThreadPool(inPool),
      mUICommandPool(inInstance.GetDevice(), inInstance.GetQueueContext()),
      mSecondaryCommandBuffersUI{
           VulkanCommandBuffer(
                inInstance.GetDevice(), mUICommandPool, VulkanCommandBuffer::Type::Secondary),
           VulkanCommandBuffer(
                inInstance.GetDevice(), mUICommandPool, VulkanCommandBuffer::Type::Secondary)} {
    for (int i = 0; i < inNumThreads; i++) {
        mThreadCommandBuffers.emplace_back(mu<ThreadCommandBufferArray>(inInstance));
    }
}

void Window::Refresh() {
    ZoneScoped;
    mSurface.ProcessCurrentSurfaceConditions(mInstance->GetPhysicalDevice());
    SDL_Event event;
    /// @todo This seems a little fishy TBH
    while (mSurface.GetExtent().height <= 0 or mSurface.GetExtent().width <= 0) {
        SDL_PollEvent(&event);
        mSurface.ProcessCurrentSurfaceConditions(mInstance->GetPhysicalDevice());
    }
    mSwapChain = VulkanSwapChain(
         mInstance->GetDevice(), mInstance->GetQueueContext(), mSurface, mSwapChain);
    mSwapChainImages = mSwapChain.GetVulkanImages(mInstance->GetDevice(), mSurface);
}

namespace Jkr {
std::array<VulkanCommandBuffer, 2U> &Window::GetCommandBuffers(ParameterContext inParameter) {
    switch (inParameter) {
        case Window_base::None:
            return mCommandBuffers;
        case Window_base::Immediate:
            return mInstance->GetUtilCommandBuffers();
        case Window_base::UI:
            return mSecondaryCommandBuffersUI;
        default:
            return mThreadCommandBuffers[inParameter]->mCommandBuffers;
    }
}

void Window::Wait() {
    mFences[mCurrentFrame].Wait();
    mFences[mCurrentFrame].Reset();
}

void Window::AcquireImage() {
    std::pair<uint32_t, uint32_t> SwapChainResult =
         mSwapChain.AcquireNextImage(mImageAvailableSemaphores[mCurrentFrame]);
    mAcquiredImageIndex = SwapChainResult.second;
}

void Window::BeginRecording() {
    mCommandBuffers[mCurrentFrame].Reset();
    mCommandBuffers[mCurrentFrame].Begin();
}

void Window::EndRecording() { mCommandBuffers[mCurrentFrame].End(); }

void Window::BeginDraws(
     float r = 0.1f, float g = 0.1f, float b = 0.1f, float a = 0.1f, float d = 1.0f) {
    std::array<float, 5> ClearValues = {r, g, b, a, d};
    mCommandBuffers[mCurrentFrame]
         .BeginRenderPass<VulkanCommandBuffer::RenderPassBeginContext::SecondaryCommandBuffers>(
              mRenderPass,
              {mOffscreenFrameSize.x, mOffscreenFrameSize.y},
              *mFrameBuffers[mAcquiredImageIndex],
              ClearValues);
}

void Window::EndDraws() { mCommandBuffers[mCurrentFrame].EndRenderPass(); }

void Window::BlitImage() {
    using namespace vk;
    auto &AquiredImage = mSwapChainImages[mAcquiredImageIndex];
    ImageSubresourceLayers SrcSubLayers(ImageAspectFlagBits::eColor, 0, 0, 1);
    ImageSubresourceLayers DstSubLayers(ImageAspectFlagBits::eColor, 0, 0, 1);
    ImageBlit Blit;
    Blit.setSrcSubresource(SrcSubLayers);
    Blit.setDstSubresource(DstSubLayers);
    Blit.srcOffsets[1].x = mOffscreenFrameSize.x;
    Blit.srcOffsets[1].y = mOffscreenFrameSize.y;
    Blit.srcOffsets[1].z = 1;
    Blit.dstOffsets[1].x = AquiredImage.GetImageExtent().width;
    Blit.dstOffsets[1].y = AquiredImage.GetImageExtent().height;
    Blit.dstOffsets[1].z = 1;
    auto &cmd            = mCommandBuffers[mCurrentFrame].GetCommandBufferHandle();
    AquiredImage.CmdTransitionImageLayout(mCommandBuffers[mCurrentFrame],
                                          vk::ImageLayout::eUndefined,
                                          vk::ImageLayout::eTransferDstOptimal,
                                          // TODO Improve this
                                          vk::PipelineStageFlagBits::eAllGraphics,
                                          vk::PipelineStageFlagBits::eTransfer,
                                          vk::AccessFlagBits::eMemoryRead,
                                          vk::AccessFlagBits::eMemoryWrite);

    cmd.blitImage(mOffscreenImages[mCurrentFrame],
                  vk::ImageLayout::eTransferSrcOptimal,
                  AquiredImage.GetImageHandle(),
                  vk::ImageLayout::eTransferDstOptimal,
                  Blit,
                  vk::Filter::eLinear);

    AquiredImage.CmdTransitionImageLayout(mCommandBuffers[mCurrentFrame],
                                          vk::ImageLayout::eTransferDstOptimal,
                                          vk::ImageLayout::ePresentSrcKHR,
                                          vk::PipelineStageFlagBits::eColorAttachmentOutput |
                                               vk::PipelineStageFlagBits::eTransfer,
                                          vk::PipelineStageFlagBits::eBottomOfPipe,
                                          vk::AccessFlagBits::eMemoryWrite,
                                          vk::AccessFlagBits::eMemoryRead);
}
void Window::Present() {
    mInstance->GetGraphicsQueue().Submit<SubmitContext::ColorAttachment>(
         mImageAvailableSemaphores[mCurrentFrame],
         mRenderFinishedSemaphores[mCurrentFrame],
         mFences[mCurrentFrame],
         mCommandBuffers[mCurrentFrame]);
    uint32_t Result = mInstance->GetGraphicsQueue().Present<SubmitContext::ColorAttachment>(
         mSwapChain, mRenderFinishedSemaphores[mCurrentFrame], mAcquiredImageIndex);

    if (mSwapChain.ImageIsNotOptimal(Result)) {
        Refresh();
    } else {
        mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;
    }
}

void Window::Submit() {
    mInstance->GetGraphicsQueue().Submit<SubmitContext::ColorAttachment>(
         mCommandBuffers[mCurrentFrame], mFences[mCurrentFrame]);
    mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;
}

void Window::BeginUIs() {
    mSecondaryCommandBuffersUI[mCurrentFrame]
         .Begin<VulkanCommandBuffer::BeginContext::ContinueRenderPassAndOneTimeSubmit>(
              mRenderPass, 0, *mFrameBuffers[mAcquiredImageIndex]);

    SetViewport(0,
                0,
                mOffscreenFrameSize.x,
                mOffscreenFrameSize.y,
                0.0f,
                1.0f,
                Window::ParameterContext::UI);
    SetScissor(0, 0, mOffscreenFrameSize.x, mOffscreenFrameSize.y, Window::ParameterContext::UI);
}

void Window::EndUIs() { mSecondaryCommandBuffersUI[mCurrentFrame].End(); }

void Window::ExecuteUIs() {
    mCommandBuffers[mCurrentFrame].ExecuteCommands(mSecondaryCommandBuffersUI[mCurrentFrame]);
}

using namespace Jkr;
void Window::BeginShadowPass(int inI, float ind) {
    mCommandBuffers[mCurrentFrame].BeginRenderPass(
         mShadowPass->GetRenderPass(),
         vk::Extent2D(mOffscreenFrameSize.x, mOffscreenFrameSize.y),
         mShadowPass->GetFrameBuffer(inI),
         {1.0f, 1.0f, 1.0f, 1.0f, ind});
}
void Window::EndShadowPass() { mCommandBuffers[mCurrentFrame].EndRenderPass(); }

void Window::BuildArbritaryPasses(int inCount, int inWidth, int inHeight) {
    for (int i = 0; i < inCount; ++i) {
        mArbritaryPasses.push_back(mu<ArbritaryPass>(*mInstance, inWidth, inHeight));
    }
}

void Window::BeginArbritaryPass(int index, int inWidth, int inHeight, glm::vec4 inClearValue) {
    mCommandBuffers[mCurrentFrame].BeginRenderPass(
         *mArbritaryPasses[index]->mRenderpass,
         vk::Extent2D{(uint32_t)inWidth, (uint32_t)inHeight},
         *mArbritaryPasses[index]->mFrameBuffer,
         std::to_array({inClearValue.x, inClearValue.y, inClearValue.z, inClearValue.w, 1.0f}));
}

void Window::EndArbritaryPass() { mCommandBuffers[mCurrentFrame].EndRenderPass(); }

void Window::ExecuteThreadCommandBuffer(int inThreadId) {
    auto &cmd = GetCommandBuffers((Renderer::CmdParam)inThreadId)[mCurrentFrame];
    mCommandBuffers[mCurrentFrame].ExecuteCommands(cmd);
}

void Window::BeginThreadCommandBuffer(int inThreadId) {
    mThreadCommandBuffers[inThreadId]->mCommandBuffers[mCurrentFrame].Reset();
    mThreadCommandBuffers[inThreadId]
         ->mCommandBuffers[mCurrentFrame]
         .Begin<VulkanCommandBuffer::BeginContext::ContinueRenderPassAndOneTimeSubmit>(
              mRenderPass, 0, *mFrameBuffers[mAcquiredImageIndex]);
}

void Window::EndThreadCommandBuffer(int inThreadId) {
    mThreadCommandBuffers[inThreadId]->mCommandBuffers[mCurrentFrame].End();
}

void Window::BuildShadowPass() {
    if (not mShadowPass) {
        mShadowPass = mu<ShadowPass>(*mInstance, mOffscreenFrameSize.x, mOffscreenFrameSize.y);
    }
}

void Window::BuildDeferredPass() {
    mDeferredPass = mu<DeferredPass>(
         *mInstance, mOffscreenFrameSize.x, mOffscreenFrameSize.y, mMaxFramesInFlight);
}

void Window::PrepareDeferredPass(Renderer::_3D::Simple3D &inCompositionSimple3D,
                                 Renderer::_3D::World3D &inWorld) {
    mDeferredPass->Prepare(inCompositionSimple3D, inWorld);
}

void Window::ExecuteDeferredComposition(Renderer::_3D::Simple3D &inCompositionSimple3D,
                                        Renderer::_3D::World3D &inWorld) {
    mDeferredPass->ExecuteDeferredComposition(*this, inCompositionSimple3D, inWorld);
}
void Window::BeginDeferredDraws(float r, float g, float b, float a, float d) {
    mDeferredPass->BeginDeferred(*this, r, g, b, a, d);
}
void Window::EndDeferredDraws() { mDeferredPass->EndDeferred(*this); }

void Window::PresentDeferred() {
    using namespace vk;
    auto &AquiredImage = mSwapChainImages[mAcquiredImageIndex];
    ImageSubresourceLayers SrcSubLayers(ImageAspectFlagBits::eColor, 0, 0, 1);
    ImageSubresourceLayers DstSubLayers(ImageAspectFlagBits::eColor, 0, 0, 1);
    ImageBlit Blit;
    Blit.setSrcSubresource(SrcSubLayers);
    Blit.setDstSubresource(DstSubLayers);
    Blit.srcOffsets[1].x = mOffscreenFrameSize.x;
    Blit.srcOffsets[1].y = mOffscreenFrameSize.y;
    Blit.srcOffsets[1].z = 1;
    Blit.dstOffsets[1].x = mWidth;
    Blit.dstOffsets[1].y = mHeight;
    Blit.dstOffsets[1].z = 1;
    auto &cmd            = mCommandBuffers[mCurrentFrame].GetCommandBufferHandle();

    AquiredImage.CmdTransitionImageLayout(mCommandBuffers[mCurrentFrame],
                                          vk::ImageLayout::eUndefined,
                                          vk::ImageLayout::eTransferDstOptimal,
                                          vk::PipelineStageFlagBits::eAllGraphics,
                                          vk::PipelineStageFlagBits::eTransfer,
                                          vk::AccessFlagBits::eMemoryRead,
                                          vk::AccessFlagBits::eMemoryWrite);

    mDeferredPass->GetDeferredCompositionImage().mUniformImagePtr->CmdTransitionImageLayout(
         mCommandBuffers[mCurrentFrame],
         vk::ImageLayout::eShaderReadOnlyOptimal,
         vk::ImageLayout::eTransferSrcOptimal,
         vk::PipelineStageFlagBits::eColorAttachmentOutput,
         vk::PipelineStageFlagBits::eTransfer,
         vk::AccessFlagBits::eMemoryWrite,
         vk::AccessFlagBits::eMemoryRead);

    cmd.blitImage(mDeferredPass->GetDeferredCompositionImage().mUniformImagePtr->GetImageHandle(),
                  vk::ImageLayout::eTransferSrcOptimal,
                  AquiredImage.GetImageHandle(),
                  vk::ImageLayout::eTransferDstOptimal,
                  Blit,
                  vk::Filter::eLinear);

    AquiredImage.CmdTransitionImageLayout(mCommandBuffers[mCurrentFrame],
                                          vk::ImageLayout::eTransferDstOptimal,
                                          vk::ImageLayout::ePresentSrcKHR,
                                          vk::PipelineStageFlagBits::eColorAttachmentOutput |
                                               vk::PipelineStageFlagBits::eTransfer,
                                          vk::PipelineStageFlagBits::eBottomOfPipe,
                                          vk::AccessFlagBits::eMemoryWrite,
                                          vk::AccessFlagBits::eMemoryRead);

    mCommandBuffers[mCurrentFrame].End();
    mInstance->GetGraphicsQueue().Submit<SubmitContext::ColorAttachment>(
         mImageAvailableSemaphores[mCurrentFrame],
         mRenderFinishedSemaphores[mCurrentFrame],
         mFences[mCurrentFrame],
         mCommandBuffers[mCurrentFrame]);

    uint32_t Result = mInstance->GetGraphicsQueue().Present<SubmitContext::ColorAttachment>(
         mSwapChain, mRenderFinishedSemaphores[mCurrentFrame], mAcquiredImageIndex);

    if (mSwapChain.ImageIsNotOptimal(Result)) {
        Refresh();
    } else {
        mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;
    }
}
void Window::BeginImmediate() {
    mInstance->GetUtilCommandBufferFences()[mCurrentFrame]->Wait();
    mInstance->GetUtilCommandBufferFences()[mCurrentFrame]->Reset();
    mInstance->GetUtilCommandBuffers()[mCurrentFrame].Begin();
}

void Window::EndImmediate() {
    mInstance->GetUtilCommandBuffers()[mCurrentFrame].End();
    mInstance->GetGraphicsQueue().Submit<SubmitContext::SingleTime>(mInstance->GetUtilCommandBuffers()[mCurrentFrame], *mInstance->GetUtilCommandBufferFences()[mCurrentFrame]);
}

} // namespace Jkr