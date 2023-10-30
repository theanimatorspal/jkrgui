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

	using ImageResult = std::pair<uint32_t, uint32_t>;

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
		uint32_t mGraphicsQueueIndex = 0;
		uint32_t mPresentQueueIndex = 0;
		uint32_t mComputeQueueIndex = 0;
		std::vector<vk::QueueFamilyProperties> mQueueFamilyProperties;
	};

}
