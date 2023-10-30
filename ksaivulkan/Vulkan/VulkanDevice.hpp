#pragma once
#include "VulkanPhysicalDevice.hpp"
#include "VulkanQueueContext.hpp"

namespace ksai {

	class VulkanDevice
	{
	public:
		VulkanDevice(const VulkanPhysicalDevice& inPhysicalDevice, const VulkanQueueContext& inQueueContext);
		~VulkanDevice();
		void Wait() const;
		inline constexpr const auto& GetDeviceHandle() const { return mDevice; }
		inline constexpr const auto& GetPhysicalDeviceHandle() const { return mPhysicalDevice; }
	private:
		std::vector<vk::QueueFamilyProperties> mQueueFamilyProperties;
		const vk::PhysicalDevice& mPhysicalDevice;
		vk::Device mDevice;
	};

}
