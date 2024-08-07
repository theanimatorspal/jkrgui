﻿#include "Window_base.hpp"
#include "SDLWindow.hpp"

Jkr::Window_base::Window_base(const Instance &inInstance, std::string_view inTitle, int inHeight, int inWidth) : mInstance(&inInstance) {
    auto iptr = &inInstance.GetVulkanInstance();
    auto dptr = &inInstance.GetDevice();
    SDLWindow::Init({inTitle, inHeight, inWidth});
    mSurface.Init({iptr, this->GetWindowHandle()});
    mSurface.ProcessCurrentSurfaceConditions(inInstance.GetPhysicalDevice()); // TODO Make this better
    mSwapChain.Init({
         .mDevice       = dptr,
         .mQueueContext = &inInstance.GetQueueContext(),
         .mSurface      = &mSurface,
    });
    mSwapChainImages = mSwapChain.GetVulkanImages(inInstance.GetDevice(), mSurface);
    mColorImageRenderTarget.Init({dptr, ImageContext::ColorAttach, {}, {}, &mSurface, 4});
    mDepthImage.Init({dptr, ImageContext::DepthImage, {}, {}, &mSurface, 4});
    mRenderPass = std::move(VulkanRenderPass<ksai::RenderPassContext::MSAA>(
         mInstance->GetDevice(), mSurface, mColorImageRenderTarget, mDepthImage, vk::SampleCountFlagBits::e4));
    for (auto &semaphore : mImageAvailableSemaphores)
        semaphore.Init({dptr});
    for (auto &semaphore : mRenderFinishedSemaphores)
        semaphore.Init({dptr});
    for (auto &fence : mFences)
        fence.Init({dptr});
    mCommandPool.Init({dptr, &inInstance.GetQueueContext()});
    for (auto &cmdbuf : mCommandBuffers)
        cmdbuf.Init({dptr, &mCommandPool});
    ksai_print("SwapChainImagesSize: %d", mSwapChainImages.size());

    for (int i = 0; i < mSwapChainImages.size(); i++) {
        mFrameBuffers.emplace_back(MakeUp<FrameBufferType>(
             inInstance.GetDevice(), mRenderPass, mColorImageRenderTarget, mDepthImage, mSwapChainImages[i]));
    }
}

void Jkr::Window_base::SetScissor(int inX, int inY, int inW, int inH, ParameterContext inContext) {
    GetCommandBuffers(inContext)[mCurrentFrame].GetCommandBufferHandle().setScissor(
         0, vk::Rect2D{vk::Offset2D{inX, inY}, vk::Extent2D{(ui)inW, (ui)inH}});
}

void Jkr::Window_base::SetDefaultScissor(ParameterContext inContext) {
    vk::Rect2D Rect(vk::Offset2D(0), mDepthImage.GetImageExtent());
    this->GetCommandBuffers(inContext)[mCurrentFrame].GetCommandBufferHandle().setScissor(0, Rect);
}

void Jkr::Window_base::SetViewport(int inX, int inY, int inW, int inH, float inMind, float inMaxD, ParameterContext inContext) {
    GetCommandBuffers(inContext)[mCurrentFrame].GetCommandBufferHandle().setViewport(
         0, vk::Viewport(inX, inY, inW, inH, inMind, inMaxD));
}

void Jkr::Window_base::SetDefaultViewport(ParameterContext inContext) {
    auto extent = mDepthImage.GetImageExtent();
    SetViewport(0.0f, 0.0f, static_cast<float>(extent.width), static_cast<float>(extent.height), 0.0f, 1.0f, inContext);
}