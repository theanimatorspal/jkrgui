#include "Window.hpp"
#include "Offscreen.hpp"
#include "Renderers/Renderer_base.hpp"
#include "Renderers/ThreeD/Simple3D.hpp"
#include "Renderers/ThreeD/World3D.hpp"
#include "VulkanCommandBuffer.hpp"
#include "vulkan/vulkan_enums.hpp"
#include <Vendor/Tracy/tracy/Tracy.hpp>

using namespace Jkr;

Jkr::Window::Window(const Instance &inInstance,
                    const sv inTitle,
                    ui inHeight,
                    ui inWidth,
                    ui inNumThreads,
                    std::span<ui> inPerThreadBuffers, // TODO remove this
                    optref<ksai::ThreadPool> inPool)
    : Window_base(inInstance, inTitle, inHeight, inWidth),
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

void Jkr::Window::Refresh() {
    ZoneScoped;
    mSurface.ProcessCurrentSurfaceConditions(mInstance->GetPhysicalDevice());
    mSwapChain = VulkanSwapChain(
         mInstance->GetDevice(), mInstance->GetQueueContext(), mSurface, mSwapChain);
    mSwapChainImages = mSwapChain.GetVulkanImages(mInstance->GetDevice(), mSurface);
    mColorImageRenderTarget =
         VulkanImage(mInstance->GetDevice(), mSurface, 4, ImageContext::ColorAttach);
    mDepthImage = VulkanImage(mInstance->GetDevice(), mSurface, 4, ImageContext::DepthImage);

    for (int i = 0; i < mSwapChainImages.size(); i++) {
        mFrameBuffers[i] = MakeUp<FrameBufferType>(mInstance->GetDevice(),
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
const std::array<VulkanCommandBuffer, 2U> &
Jkr::Window::GetCommandBuffers(ParameterContext inParameter) const {
    switch (inParameter) {
        case Jkr::Window_base::None:
            return mCommandBuffers;
        case Jkr::Window_base::UI:
            return mSecondaryCommandBuffersUI;
        default:
            return mThreadCommandBuffers[inParameter]->mCommandBuffers;
    }
}

void Jkr::Window::BeginUpdates() {
    mFences[mCurrentFrame].Wait();
    mFences[mCurrentFrame].Reset();
}

void Window::EndUpdates() {
    std::pair<uint32_t, uint32_t> SwapChainResult =
         mSwapChain.AcquireNextImage(mImageAvailableSemaphores[mCurrentFrame]);
    mAcquiredImageIndex = SwapChainResult.second;
}

void Window::BeginDispatches() {
    mCommandBuffers[mCurrentFrame].Reset();
    mCommandBuffers[mCurrentFrame].Begin();
}

void Window::EndDispatches() {}

void Window::BeginDraws(
     float r = 0.1f, float g = 0.1f, float b = 0.1f, float a = 0.1f, float d = 1.0f) {
    std::array<float, 5> ClearValues = {r, g, b, a, d};
    mCommandBuffers[mCurrentFrame]
         .BeginRenderPass<VulkanCommandBuffer::RenderPassBeginContext::SecondaryCommandBuffers>(
              mRenderPass, mSurface.GetExtent(), *mFrameBuffers[mAcquiredImageIndex], ClearValues);
}

void Window::EndDraws() { mCommandBuffers[mCurrentFrame].EndRenderPass(); }

void Window::Present() {
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

void Window::BeginUIs() {
    mSecondaryCommandBuffersUI[mCurrentFrame]
         .Begin<VulkanCommandBuffer::BeginContext::ContinueRenderPassAndOneTimeSubmit>(
              mRenderPass, 0, *mFrameBuffers[mAcquiredImageIndex]);

    // TODO Replace this with Our Window Functions
    mSecondaryCommandBuffersUI[mCurrentFrame].SetViewport(mSurface);
    mSecondaryCommandBuffersUI[mCurrentFrame].SetScissor(mSurface);
}

void Window::EndUIs() { mSecondaryCommandBuffersUI[mCurrentFrame].End(); }

void Window::ExecuteUIs() {
    mCommandBuffers[mCurrentFrame].ExecuteCommands(mSecondaryCommandBuffersUI[mCurrentFrame]);
}

using namespace Jkr;
void Window::BeginShadowPass(float ind) {
    mCommandBuffers[mCurrentFrame].BeginRenderPass(mShadowPass->GetRenderPass(),
                                                   vk::Extent2D(mFrameSize.x, mFrameSize.y),
                                                   mShadowPass->GetFrameBuffer(),
                                                   {1.0f, 1.0f, 1.0f, 1.0f, ind});
}
void Window::EndShadowPass() {

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

void Window::BuildShadowPass(ui inWidth, ui inHeight) {
    mShadowPass = mu<ShadowPass>(*mInstance, inWidth, inHeight);
    mFrameSize  = glm::uvec2(inWidth, inHeight);
}

void Window::BuildDeferredPass(ui inWidth, ui inHeight) {
    mDeferredPass = mu<DeferredPass>(*mInstance, inWidth, inHeight, mMaxFramesInFlight);
    mFrameSize    = glm::uvec2(inWidth, inHeight);
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
    auto d             = GetWindowDimension();
    auto &AquiredImage = mSwapChainImages[mAcquiredImageIndex];
    ImageSubresourceLayers SrcSubLayers(ImageAspectFlagBits::eColor, 0, 0, 1);
    ImageSubresourceLayers DstSubLayers(ImageAspectFlagBits::eColor, 0, 0, 1);
    ImageBlit Blit;
    Blit.setSrcSubresource(SrcSubLayers);
    Blit.setDstSubresource(DstSubLayers);
    Blit.srcOffsets[1].x = mFrameSize.x;
    Blit.srcOffsets[1].y = mFrameSize.y;
    Blit.srcOffsets[1].z = 1;
    Blit.dstOffsets[1].x = d.x;
    Blit.dstOffsets[1].y = d.y;
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

} // namespace Jkr