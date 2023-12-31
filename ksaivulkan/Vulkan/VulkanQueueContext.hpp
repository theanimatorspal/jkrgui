#pragma once
#include "VulkanSurface.hpp"

namespace ksai {

	enum class QueueContext
	{
		Graphics,
		Present,
		Compute
	};

	enum class SubmitContext
	{
		SingleTime,
		ColorAttachment,
		DepthAttachment
	};

	template <size_t MaxFramesInFlight>

	using ImageResult = p<ui, ui>;

	class VulkanQueueContext
	{
	public:
		VulkanQueueContext(const VulkanPhysicalDevice& inDevice, const VulkanSurface& inSurface);
		~VulkanQueueContext();
		void ProcessSurfaceContextModes();
		GETTER GetGraphicsQueueFamilyIndex() const { return mGraphicsQueueIndex; }
		GETTER GetPresentQueueFamilyIndex() const { return mPresentQueueIndex; }
		GETTER GetComputeQueueFamilyIndex() const { return mComputeQueueIndex; }
	private:
		const vk::PhysicalDevice& mPhysicalDevice;
		const vk::SurfaceKHR& mSurface;
		const SDL_Window* mWindow;
		ui mGraphicsQueueIndex = 0;
		ui mPresentQueueIndex = 0;
		ui mComputeQueueIndex = 0;
		v<vk::QueueFamilyProperties> mQueueFamilyProperties;
	};

}
