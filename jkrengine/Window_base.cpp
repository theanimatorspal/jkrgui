﻿#include "Window_base.hpp"
#include "SDLWindow.hpp"

using namespace Jkr;
Window_base::Window_base(Instance &inInstance,
                         std::string_view inTitle,
                         int inHeight,
                         int inWidth,
                         int inOffscreenFrameHeight,
                         int inOffscreenFrameWidth)
    : mInstance(&inInstance) {
    mOffscreenFrameSize.x = inOffscreenFrameWidth;
    mOffscreenFrameSize.y = inOffscreenFrameHeight;
    auto iptr             = &inInstance.GetVulkanInstance();
    auto dptr             = &inInstance.GetDevice();
    SDLWindow::Init({inTitle, inHeight, inWidth});
    mSurface.Init({iptr, this->GetWindowHandle()});
    mSurface.ProcessCurrentSurfaceConditions(
         inInstance.GetPhysicalDevice()); // TODO Make this better
    mSwapChain.Init({
         .mDevice       = dptr,
         .mQueueContext = &inInstance.GetQueueContext(),
         .mSurface      = &mSurface,
    });
    mSwapChainImages = mSwapChain.GetVulkanImages(inInstance.GetDevice(), mSurface);
    mColorImageRenderTarget.Init({dptr,
                                  ImageContext::ColorAttach,
                                  mOffscreenFrameSize.x,
                                  mOffscreenFrameSize.y,
                                  &mSurface,
                                  4});
    mColorImageRenderTarget.SetDebugUtilsName("ColorImageRenderTarget");
    mDepthImage.Init({dptr,
                      ImageContext::DepthImage,
                      mOffscreenFrameSize.x,
                      mOffscreenFrameSize.y,
                      nullptr,
                      4});
    mDepthImage.SetDebugUtilsName("DepthImage");
    mRenderPass =
         std::move(VulkanRenderPass<ksai::RenderPassContext::MSAA>(mInstance->GetDevice(),
                                                                   mSurface.GetSurfaceImageFormat(),
                                                                   mColorImageRenderTarget,
                                                                   mDepthImage,
                                                                   vk::SampleCountFlagBits::e4));
    for (auto &semaphore : mImageAvailableSemaphores)
        semaphore.Init({dptr});
    for (auto &semaphore : mRenderFinishedSemaphores)
        semaphore.Init({dptr});
    for (auto &fence : mFences)
        fence.Init({dptr});
    mCommandPool.Init({dptr, &inInstance.GetQueueContext()});
    for (auto &cmdbuf : mCommandBuffers)
        cmdbuf.Init({dptr, &mCommandPool});
    Log("SwapChainImagesSize: " + std::to_string(mSwapChainImages.size()));

    mOffscreenImages.resize(mSwapChainImages.size());
    for (int i = 0; i < mSwapChainImages.size(); i++) {
        mOffscreenImages[i].Init(ksai::VulkanImageVMA::CreateInfo{
             &mInstance->GetVMA(),
             &mInstance->GetDevice(),
             mOffscreenFrameSize.x, // TODO Make this better <from user>
             mOffscreenFrameSize.y,
             ksai::ImageContext::Default,
             4,
             1,
             1,
             1,
             false,
             vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc,
             vk::ImageLayout::eUndefined,
             vk::Format::eR8G8B8A8Unorm});
        mFrameBuffers.emplace_back(MakeUp<FrameBufferType>(inInstance.GetDevice(),
                                                           mRenderPass,
                                                           mColorImageRenderTarget,
                                                           mDepthImage,
                                                           mOffscreenImages[i]));
    }
}
Window_base::Window_base(Instance &inInstance,
                         int inOffscreenFrameHeight,
                         int inOffscreenFrameWidth)
    : mInstance(&inInstance) {
    mOffscreenFrameSize.x = inOffscreenFrameWidth;
    mOffscreenFrameSize.y = inOffscreenFrameHeight;
    auto iptr             = &inInstance.GetVulkanInstance();
    auto dptr             = &inInstance.GetDevice();
    mCommandPool.Init({dptr, &inInstance.GetQueueContext()});
    for (auto &cmdbuf : mCommandBuffers)
        cmdbuf.Init({dptr, &mCommandPool});
    for (auto &semaphore : mImageAvailableSemaphores)
        semaphore.Init({dptr});
    for (auto &semaphore : mRenderFinishedSemaphores)
        semaphore.Init({dptr});
    for (auto &fence : mFences)
        fence.Init({dptr});
    mColorImageRenderTarget.Init({dptr,
                                  ImageContext::ColorAttach,
                                  mOffscreenFrameSize.x,
                                  mOffscreenFrameSize.y,
                                  &mSurface,
                                  4});

    mDepthImage.Init({dptr,
                      ImageContext::DepthImage,
                      mOffscreenFrameSize.x,
                      mOffscreenFrameSize.y,
                      nullptr,
                      4});

    mRenderPass =
         std::move(VulkanRenderPass<ksai::RenderPassContext::MSAA>(mInstance->GetDevice(),
                                                                   vk::Format::eR8G8B8A8Unorm,
                                                                   mColorImageRenderTarget,
                                                                   mDepthImage,
                                                                   vk::SampleCountFlagBits::e4));
    mOffscreenImages.resize(1);
    for (int i = 0; i < mOffscreenImages.size(); i++) {
        mOffscreenImages[i].Init(ksai::VulkanImageVMA::CreateInfo{
             &mInstance->GetVMA(),
             &mInstance->GetDevice(),
             mOffscreenFrameSize.x,
             mOffscreenFrameSize.y,
             ksai::ImageContext::Default,
             4,
             1,
             1,
             1,
             false,
             vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc,
             vk::ImageLayout::eUndefined,
             vk::Format::eR8G8B8A8Unorm});
        mFrameBuffers.emplace_back(MakeUp<FrameBufferType>(inInstance.GetDevice(),
                                                           mRenderPass,
                                                           mColorImageRenderTarget,
                                                           mDepthImage,
                                                           mOffscreenImages[i]));
    }
}

void Window_base::SetScissor(int inX, int inY, int inW, int inH, ParameterContext inContext) {
    if (inX < 0) {
        inW = inW + inX;
        inX = 0;
    } else if (inY < 0) {
        inH = inH + inY;
        inY = 0;
    }

    if (inW > 0 && inH > 0) {
        GetCommandBuffers(inContext)[mCurrentFrame].GetCommandBufferHandle().setScissor(
             0, vk::Rect2D{vk::Offset2D{inX, inY}, vk::Extent2D{(ui)inW, (ui)inH}});
    } else {
        GetCommandBuffers(inContext)[mCurrentFrame].GetCommandBufferHandle().setScissor(
             0, vk::Rect2D{vk::Offset2D{inX, inY}, vk::Extent2D{(ui)1, (ui)1}});
    }
}

void Window_base::SetDefaultScissor(ParameterContext inContext) {
    vk::Rect2D Rect(vk::Offset2D(0), mDepthImage.GetImageExtent());
    this->GetCommandBuffers(inContext)[mCurrentFrame].GetCommandBufferHandle().setScissor(0, Rect);
}

void Window_base::SetViewport(
     int inX, int inY, int inW, int inH, float inMind, float inMaxD, ParameterContext inContext) {
    if (inX < 0) {
        inW = inW + inX;
        inX = 0;
    } else if (inY < 0) {
        inH = inH + inY;
        inY = 0;
    }
    if (inW > 0 && inH > 0) {
        GetCommandBuffers(inContext)[mCurrentFrame].GetCommandBufferHandle().setViewport(
             0, vk::Viewport(inX, inY, inW, inH, inMind, inMaxD));
    } else {
        GetCommandBuffers(inContext)[mCurrentFrame].GetCommandBufferHandle().setViewport(
             0, vk::Viewport(inX, inY, 1, 1, inMind, inMaxD));
    }
}

void Window_base::SetDefaultViewport(ParameterContext inContext) {
    auto extent = mDepthImage.GetImageExtent();
    SetViewport(0.0f,
                0.0f,
                static_cast<float>(extent.width),
                static_cast<float>(extent.height),
                0.0f,
                1.0f,
                inContext);
}