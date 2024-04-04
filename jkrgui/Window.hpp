#pragma once
#include "SDLWindow.hpp"

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
          GETTER& GetCurrentFrame() const { return mCurrentFrame; }
          GETTER& GetInstance() const { return mInstance; }
          GETTER& GetRenderPass() const { return mRenderPass; }
          GETTER& GetSwapChainImages() const { return mSwapChainImages; }

          SETTER SetScissor(const vk::ArrayProxy<vk::Rect2D>& inScissor, ParameterContext inContext = ParameterContext::UI);
          SETTER ResetScissor(ParameterContext inContext = ParameterContext::UI);
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

inline void Jkr::Window::SetScissor(const vk::ArrayProxy<vk::Rect2D>& inScissor, ParameterContext inContext) {
          this->GetCommandBuffers(inContext)[mCurrentFrame].GetCommandBufferHandle().setScissor(0, inScissor);
}
inline void Jkr::Window::ResetScissor(ParameterContext inContext) {
          vk::Rect2D Rect(vk::Offset2D(0), mDepthImage.GetImageExtent());
          this->GetCommandBuffers(inContext)[mCurrentFrame].GetCommandBufferHandle().setScissor(0, Rect);
}
