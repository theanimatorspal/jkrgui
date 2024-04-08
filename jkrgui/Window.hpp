#pragma once
#include "PainterParameter.hpp"
#include "SDLWindow.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_structs.hpp"

namespace Jkr {
class Window : public SDLWindow {
      public:
          using SwapChainVulkanImages = std::vector<VulkanImage<ImageContext::ExternalHandled>>;
          using FrameBufferType = VulkanFrameBuffer<RenderPassContext::MSAA,
                                                    3,
                                                    VulkanImage<ImageContext::ColorAttach>,
                                                    VulkanImage<ImageContext::DepthImage>,
                                                    VulkanImage<ImageContext::ExternalHandled>>;
          enum ParameterContext : int {
                    None = -3,
                    UI = -2,
                    Background = -1,
          };

      public:
          virtual const std::array<VulkanCommandBuffer, 2U>& GetCommandBuffers(ParameterContext inParameter) const { return mCommandBuffers; }
          const VulkanCommandPool& GetCommandPool() const { return mCommandPool; }
          GETTER& GetCurrentFrame() const { return mCurrentFrame; } // TODO Remove this reference
          GETTER& GetInstance() const { return mInstance; }
          GETTER& GetRenderPass() const { return mRenderPass; }
          GETTER& GetSwapChainImages() const { return mSwapChainImages; }

          SETTER SetScissor(int inX, int inY, int inW, int inH, ParameterContext inContext = ParameterContext::UI);
          SETTER SetDefaultScissor(ParameterContext inContext = ParameterContext::UI);
          SETTER SetViewport(int inX, int inY, int inW, int inH, int inMind, int inMaxD, ParameterContext inContext);
          SETTER SetDefaultViewport(ParameterContext inContext);
          // TODO Remove this
          void CmdCopySwapChainImageToBufferPostRendering(VulkanBufferBase& inbuffer);

          Window(const Instance& inInstance, std::string_view inTitle, int inHeight, int inWidth);
          ~Window() { mInstance.GetDevice().Wait(); }

      protected:
          static const int mMaxFramesInFlight = 2;
          uint32_t mCurrentFrame = 0;
          uint32_t mAcquiredImageIndex = 0;
          const Instance& mInstance;
          VulkanSurface mSurface;
          VulkanSwapChain<mMaxFramesInFlight> mSwapChain;
          VulkanImage<ImageContext::ColorAttach> mColorImageRenderTarget;
          VulkanImage<ImageContext::DepthImage> mDepthImage;
          VulkanRenderPass<RenderPassContext::MSAA> mRenderPass;
          SwapChainVulkanImages mSwapChainImages;
          std::vector<up<FrameBufferType>> mFrameBuffers;
          const std::array<VulkanSemaphore, mMaxFramesInFlight> mImageAvailableSemaphores;
          const std::array<VulkanSemaphore, mMaxFramesInFlight> mRenderFinishedSemaphores;
          std::array<VulkanFence, mMaxFramesInFlight> mFences;
          const VulkanCommandPool mCommandPool;
          const std::array<VulkanCommandBuffer, mMaxFramesInFlight> mCommandBuffers;
};

} // namespace Jkr

SETTER Jkr::Window::SetScissor(int inX, int inY, int inW, int inH, ParameterContext inContext) {
          GetCommandBuffers(inContext)[mCurrentFrame].GetCommandBufferHandle().setScissor(
           0, vk::Rect2D{vk::Offset2D{inX, inY}, vk::Extent2D{(ui)inW, (ui)inH}});
}

SETTER Jkr::Window::SetDefaultScissor(ParameterContext inContext) {
          vk::Rect2D Rect(vk::Offset2D(0), mDepthImage.GetImageExtent());
          this->GetCommandBuffers(inContext)[mCurrentFrame].GetCommandBufferHandle().setScissor(0, Rect);
}

SETTER Jkr::Window::SetViewport(int inX, int inY, int inW, int inH, int inMind, int inMaxD, ParameterContext inContext) {
          GetCommandBuffers(inContext)[mCurrentFrame].GetCommandBufferHandle().setViewport(0, vk::Viewport(inX, inY, inW, inH, inMind, inMaxD));
}

SETTER Jkr::Window::SetDefaultViewport(ParameterContext inContext) {
          auto wh = GetWindowDimension();
          SetViewport(0, 0, wh.x, wh.y, 0, 1, inContext);
}
