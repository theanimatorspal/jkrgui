#include "Window.hpp"

Jkr::Window::Window(const Instance& inInstance, std::string_view inTitle, int inHeight, int inWidth)
    : SDLWindow(inTitle, inHeight, inWidth),
      mInstance(inInstance),
      mSurface(mInstance.GetVulkanInstance(), mSDLWindowPtr),
      mSwapChain(mInstance.GetDevice(),
                 mInstance.GetQueueContext(),
                 mSurface.ProcessCurrentSurfaceConditions(mInstance.GetPhysicalDevice())
                      .ProcessCurrentSurfaceExtents(mInstance.GetPhysicalDevice())),
      mColorImageRenderTarget(mInstance.GetDevice(), mSurface, 4, ImageContext::ColorAttach),
      mDepthImage(mInstance.GetDevice(), mSurface, 4, ImageContext::DepthImage) // Here
      ,
      mRenderPass(mInstance.GetDevice(),
                  mSurface,
                  mColorImageRenderTarget,
                  mDepthImage,
                  vk::SampleCountFlagBits::e4),
      mSwapChainImages{mSwapChain.GetVulkanImages(mInstance.GetDevice(), mSurface)},
      mImageAvailableSemaphores{VulkanSemaphore(mInstance.GetDevice()),
                                VulkanSemaphore(mInstance.GetDevice())},
      mRenderFinishedSemaphores{VulkanSemaphore(mInstance.GetDevice()),
                                VulkanSemaphore(mInstance.GetDevice())},
      mFences{VulkanFence(mInstance.GetDevice()), VulkanFence(mInstance.GetDevice())},
      mCommandPool(inInstance.GetDevice(), inInstance.GetQueueContext()),
      mCommandBuffers{VulkanCommandBuffer(inInstance.GetDevice(), mCommandPool),
                      VulkanCommandBuffer(inInstance.GetDevice(), mCommandPool)} {
    ksai_print("SwapChainImagesSize: %d", mSwapChainImages.size());
    for (int i = 0; i < mSwapChainImages.size(); i++) {
        mFrameBuffers.emplace_back(MakeUp<FrameBufferType>(inInstance.GetDevice(),
                                                           mRenderPass,
                                                           mColorImageRenderTarget,
                                                           mDepthImage,
                                                           mSwapChainImages[i]));
    }
}

void Jkr::Window::CmdCopySwapChainImageToBufferPostRendering(VulkanBufferBase& inbuffer) {
    // TODO This has to be tested
    auto& CommandBuffer  = this->GetCommandBuffers(ParameterContext::None)[mCurrentFrame];
    auto& Cmd            = CommandBuffer.GetCommandBufferHandle();
    auto& srcImageHandle = mSwapChainImages[mCurrentFrame].GetImageHandle();
    auto srcImageProp    = mSwapChainImages[mCurrentFrame].GetImageProperty();
    auto& srcVulkanImage = mSwapChainImages[mCurrentFrame];
    auto srcImageExtent  = srcVulkanImage.GetImageExtent();
    vk::ImageSubresourceLayers Layer(srcImageProp.mImageAspect, 0, 0, srcImageProp.mArrayLayers);
    vk::BufferImageCopy CopyRegions(
         0, 0, 0, Layer, vk::Offset3D{}, vk::Extent3D(srcImageExtent, 1));

    srcVulkanImage.CmdTransitionImageLayout(CommandBuffer,
                                            vk::ImageLayout::eUndefined,
                                            vk::ImageLayout::eTransferSrcOptimal,
                                            vk::PipelineStageFlagBits::eLateFragmentTests,
                                            vk::PipelineStageFlagBits::eTransfer,
                                            vk::AccessFlagBits::eNone,
                                            vk::AccessFlagBits::eMemoryRead);

    Cmd.copyImageToBuffer(srcImageHandle,
                          vk::ImageLayout::eTransferSrcOptimal,
                          inbuffer.GetBufferHandle(),
                          CopyRegions);

    srcVulkanImage.CmdTransitionImageLayout(CommandBuffer,
                                            vk::ImageLayout::eTransferSrcOptimal,
                                            vk::ImageLayout::ePresentSrcKHR,
                                            vk::PipelineStageFlagBits::eTransfer,
                                            vk::PipelineStageFlagBits::eLateFragmentTests,
                                            vk::AccessFlagBits::eMemoryRead,
                                            vk::AccessFlagBits::eMemoryRead);
}

void Jkr::Window::SetScissor(int inX, int inY, int inW, int inH, ParameterContext inContext) {
    GetCommandBuffers(inContext)[mCurrentFrame].GetCommandBufferHandle().setScissor(
         0, vk::Rect2D{vk::Offset2D{inX, inY}, vk::Extent2D{(ui)inW, (ui)inH}});
}

void Jkr::Window::SetDefaultScissor(ParameterContext inContext) {
    vk::Rect2D Rect(vk::Offset2D(0), mDepthImage.GetImageExtent());
    this->GetCommandBuffers(inContext)[mCurrentFrame].GetCommandBufferHandle().setScissor(0, Rect);
}

void Jkr::Window::SetViewport(
     int inX, int inY, int inW, int inH, float inMind, float inMaxD, ParameterContext inContext) {
    GetCommandBuffers(inContext)[mCurrentFrame].GetCommandBufferHandle().setViewport(
         0, vk::Viewport(inX, inY, inW, inH, inMind, inMaxD));
}

void Jkr::Window::SetDefaultViewport(ParameterContext inContext) {
    auto extent = mDepthImage.GetImageExtent();
    SetViewport(0.0f,
                0.0f,
                static_cast<float>(extent.width),
                static_cast<float>(extent.height),
                0.0f,
                1.0f,
                inContext);
}