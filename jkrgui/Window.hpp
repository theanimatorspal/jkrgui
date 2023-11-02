#pragma once
#define SDL_MAIN_HANDLED
#include "Instance.hpp"
#include <any>
#include <functional>

namespace Jkr {
	class SDLWindow {
	public:
		SDLWindow(std::string inName, int inHeight, int inWidth);
		~SDLWindow();
		void SetSize(int inWidth, int inHeight);
		void SetTitle(std::string inString) const;
		void SetWindowBorderless();
		std::pair<int, int> GetWindowSize() const;
		GETTER GetWindowHandle() const { return mSDLWindowPtr; }
		void SetWindowShouldClose(bool inValue) { mWindowShouldClose = inValue; }
		void SetResizeCallBack(const std::function<void(void*)>& inFunction) { mResizeFunction = inFunction; }
		void ToggleWindowFullScreen();
		void Minimize();
		static auto GetWindowCurrentTime() { return SDL_GetTicks(); }
	protected:
		void* mData = nullptr;
		std::function<void(void*)> mResizeFunction = [](void*) {};
		bool mWindowShouldClose = false;
		bool mWindowIsFullScreen = false;
		std::string mName;
		int mHeight;
		int mWidth;
		SDL_Window* mSDLWindowPtr;
	};
}

namespace Jkr {
	class Window : public SDLWindow
	{
	public:
		Window(const Instance& inInstance, std::string inTitle, int inHeight, int inWidth);
		~Window() { mInstance.GetDevice().Wait(); }
		void SetDrawCallBack(const std::function<void(void*)>& inFunction) { mDrawFunction = inFunction; }
		void SetUpdateCallBack(const std::function<void(void*)>& inFunction) { mUpdateFunction = inFunction; }
		void SetComputeDispatchCallBack(const std::function<void(void*)>& inFunction) { mComputeDispatchFunction = inFunction; }
		void SetContextData(void* inData) { mData = inData; }
		void Refresh();
		void Draw(float r = 0.1f, float g = 0.1f, float b = 0.1f, float a = 0.1f, float d =  1.0f);
		GETTER& GetCurrentFrame() const { return mCurrentFrame; }
		GETTER& GetInstance() const { return mInstance; }
		GETTER& GetRenderPass() const { return mRenderPass; }
	private:
		std::function<void(void*)> mDrawFunction = [](void*) {};
		std::function<void(void*)> mUpdateFunction = [](void*) {};
		std::function<void(void*)> mComputeDispatchFunction = [](void*) {};
	private:
		static const int mMaxFramesInFlight = 2;
		using SwapChainVulkanImages = std::vector<VulkanImage<ImageContext::ExternalHandled>>;
		using FrameBufferType = VulkanFrameBuffer <
			RenderPassContext::Default, 2, VulkanImage<ImageContext::ExternalHandled>,
			VulkanImage<ImageContext::DepthImage>
		>;
	private:
		const Instance& mInstance;
		VulkanSurface mSurface;
		VulkanSwapChain<mMaxFramesInFlight> mSwapChain;
		VulkanImage<ImageContext::DepthImage> mDepthImage;
		VulkanRenderPass<RenderPassContext::Default> mRenderPass;
		SwapChainVulkanImages mSwapChainImages;
		std::array<FrameBufferType, mMaxFramesInFlight> mFrameBuffers;
	private:
		const std::array<VulkanSemaphore, mMaxFramesInFlight> mImageAvailableSemaphores;
		const std::array<VulkanSemaphore, mMaxFramesInFlight> mRenderFinishedSemaphores;
		std::array<VulkanFence, mMaxFramesInFlight> mFences;
		uint32_t mCurrentFrame = 0;
		uint32_t mAcquiredImageIndex = 0;
	};
}
