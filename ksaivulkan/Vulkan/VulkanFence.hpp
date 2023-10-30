#pragma once
#include <vulkan/vulkan.hpp>
#include "VulkanDevice.hpp"

namespace ksai {

	class VulkanFence
	{
	public:
		VulkanFence(const VulkanDevice& inDevice);
		~VulkanFence();
		void Wait();
		void Reset();
		inline constexpr const auto& GetFenceHandle() const { return mFence; }
	private:
		const vk::Device& mDevice;
		vk::Fence mFence;
	};

}
