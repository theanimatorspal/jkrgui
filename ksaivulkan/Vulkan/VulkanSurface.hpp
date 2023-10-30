#pragma once
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "VulkanInstance.hpp"
#include "VulkanPhysicalDevice.hpp"

namespace ksai {
	class VulkanSurface
	{
	public:
		VulkanSurface(const VulkanInstance& inInstance, SDL_Window* inWindow);
		~VulkanSurface();
		VulkanSurface& ProcessCurrentSurfaceConditions(const VulkanPhysicalDevice& inPhysicalDevice);
		VulkanSurface& ProcessCurrentSurfaceExtents(const VulkanPhysicalDevice& inPhysicalDevice);
		inline constexpr const auto& GetSurfaceHandle() const { return mSurface; }
		inline constexpr const auto& GetWindowHandle() const { return mWindow; }
		inline constexpr const auto& GetSurfaceCapabilities() const { return mSurfaceCapabilities; }
		inline constexpr const auto& GetSurfaceImageFormat() const { return mSurfaceImageFormat; }
		inline constexpr const auto& GetExtent() const { return mExtent; }
		inline constexpr const auto& GetPresentMode() const { return mPresentMode; }
		inline constexpr const auto& GetPreTransform() const { return mPreTransform; }
		inline constexpr const auto& GetCompositeAlpha() const { return mCompositeAlpha; }
	private:
		const vk::Instance& mInstance;
		vk::SurfaceKHR mSurface;
		SDL_Window* mWindow;
		vk::Format mSurfaceImageFormat;
		vk::SurfaceCapabilitiesKHR  mSurfaceCapabilities;
		vk::Extent2D mExtent;
		vk::PresentModeKHR mPresentMode;
		vk::SurfaceTransformFlagBitsKHR mPreTransform;
		vk::CompositeAlphaFlagBitsKHR mCompositeAlpha;
	};

}
