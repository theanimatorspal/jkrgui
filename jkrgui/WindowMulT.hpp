#include "MultiThreading.hpp"
#include <Vendor/Tracy/tracy/TracyVulkan.hpp>

namespace Jkr {
	class WindowMulT : public Window {
		using VulkanCommandBufferArray = std::array<VulkanCommandBuffer, mMaxFramesInFlight>;
	public:
		const VulkanCommandBuffer& GetSecondaryCmdBufferBackground() const override { return mSecondaryCommandBuffersBackground[mCurrentFrame]; }
		const VulkanCommandBuffer& GetSecondaryCmdBufferUI() const override { return mSecondaryCommandBuffersUI[mCurrentFrame]; }
		const VulkanCommandBuffer& GetUtilCommandBuffer() const override { return mCommandBuffers[mCurrentFrame]; }
		const VulkanCommandBufferArray& GetCommandBuffers(ParameterContext inParameter, int inThreadId = 0, int inObjId = 0) const override;

		WindowMulT(const Instance& inInstance, const sv inTitle, ui inHeight, ui inWidth, ui inNumThreads, std::span<ui> inPerThreadBuffers, optref<ksai::ThreadPool> inPool = std::nullopt);
		~WindowMulT() = default;
		virtual void Refresh() override;
		virtual void Draw(float r = 0.1f, float g = 0.1f, float b = 0.1f, float a = 0.1f, float d = 1.0f) override;
		void BeginUpdates();
		void EndUpdates();
		void BeginDispatches();
		void EndDispatches();
		void BeginDraws(float r, float g, float b, float a, float d);
		void EndDraws();
		void Present();

		void BeginUIs();
		void EndUIs();
		void ExecuteUIs();

	private:
		optref<ksai::ThreadPool> mThreadPool;
		VulkanCommandPool mBackgroundCommandPool;
		VulkanCommandPool mUICommandPool;
		VulkanCommandBufferArray mSecondaryCommandBuffersBackground;
		VulkanCommandBufferArray mSecondaryCommandBuffersUI;
		v<up<VulkanCommandBuffer>> mSecondaryCommandBuffers;
		v<up<ThreadData>> mThreadData;

		void CmdBackground();
		void CmdUI();
	public:
		void SetBackgroundCallBack(std::function<void(void*)> inFunction) { mBackgroundCallback = inFunction; }
		void SetDedicated3DCallback(std::function<void(void*)> inFunction) { mDedicated3DCallback = inFunction; }

	private:
		std::function<void(void*)> mBackgroundCallback = [](void*) {};
		std::function<void(void*)> mDedicated3DCallback = [](void*) {};
	};

} // namespace Jkr


namespace Jkr {
	inline const std::array<VulkanCommandBuffer, 2U>& Jkr::WindowMulT::GetCommandBuffers(ParameterContext inParameter, int inThreadId, int inObjId) const
	{
		switch (inParameter) {
		case Jkr::Window::None:
			return mCommandBuffers;
		case Jkr::Window::UI:
			return mSecondaryCommandBuffersUI;
		case Jkr::Window::Background:
			return mSecondaryCommandBuffersBackground;
		default:
			return mThreadData[inThreadId]->mCommandBuffers[inObjId]->mCmdBuffers;
		}
	}

	inline void Jkr::WindowMulT::BeginUpdates()
	{
		mFences[mCurrentFrame].Wait();
		mFences[mCurrentFrame].Reset();
	}

	inline void WindowMulT::EndUpdates()
	{
		std::pair<uint32_t, uint32_t> SwapChainResult = mSwapChain.AcquireNextImage(mImageAvailableSemaphores[mCurrentFrame]);
		mAcquiredImageIndex = SwapChainResult.second;
	}

	inline void WindowMulT::BeginDispatches()
	{
		mCommandBuffers[mCurrentFrame].Reset();
		mCommandBuffers[mCurrentFrame].Begin();
	}

	inline void WindowMulT::EndDispatches()
	{
	}

	inline void WindowMulT::BeginDraws(float r = 0.1f, float g = 0.1f, float b = 0.1f, float a = 0.1f, float d = 1.0f)
	{
		std::array<float, 5>
			ClearValues = { r, g, b, a, d };
		mCommandBuffers[mCurrentFrame].BeginRenderPass<VulkanCommandBuffer::RenderPassBeginContext::SecondaryCommandBuffers>(
			mRenderPass,
			mSurface,
			*mFrameBuffers[mAcquiredImageIndex],
			ClearValues);
	}

	inline void WindowMulT::EndDraws()
	{
		mCommandBuffers[mCurrentFrame].EndRenderPass();
	}

	inline void WindowMulT::Present()
	{
		mCommandBuffers[mCurrentFrame].End();

		mInstance.GetGraphicsQueue().Submit<SubmitContext::ColorAttachment>(
			mImageAvailableSemaphores[mCurrentFrame],
			mRenderFinishedSemaphores[mCurrentFrame],
			mFences[mCurrentFrame],
			mCommandBuffers[mCurrentFrame]);
		uint32_t Result = mInstance.GetGraphicsQueue().Present<mMaxFramesInFlight, SubmitContext::ColorAttachment>(
			mSwapChain,
			mRenderFinishedSemaphores[mCurrentFrame],
			mAcquiredImageIndex);

		if (mSwapChain.ImageIsNotOptimal(Result)) {
			Refresh();
		} else {
			mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;
		}
	}

	inline void WindowMulT::BeginUIs()
	{
		mSecondaryCommandBuffersUI[mCurrentFrame].Begin<VulkanCommandBuffer::BeginContext::ContinueRenderPassAndOneTimeSubmit>(
			mRenderPass,
			0,
			*mFrameBuffers[mAcquiredImageIndex]);

		mSecondaryCommandBuffersUI[mCurrentFrame].SetViewport(mSurface);
		mSecondaryCommandBuffersUI[mCurrentFrame].SetScissor(mSurface);
	}

	inline void WindowMulT::EndUIs()
	{
		mSecondaryCommandBuffersUI[mCurrentFrame].End();
	}

	inline void WindowMulT::ExecuteUIs()
	{
		mCommandBuffers[mCurrentFrame].ExecuteCommands(mSecondaryCommandBuffersUI[mCurrentFrame]);
	}
}
