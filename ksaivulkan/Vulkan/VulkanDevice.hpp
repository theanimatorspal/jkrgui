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
		GETTER& GetDeviceHandle() const { return mDevice; }
		GETTER& GetPhysicalDeviceHandle() const { return mPhysicalDevice; }
	private:
		std::vector<vk::QueueFamilyProperties> mQueueFamilyProperties;
		const vk::PhysicalDevice& mPhysicalDevice;
		vk::Device mDevice;
	};

}
