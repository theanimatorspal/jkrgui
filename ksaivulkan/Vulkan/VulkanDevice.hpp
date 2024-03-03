#pragma once
#include "VulkanPhysicalDevice.hpp"
#include "VulkanQueueContext.hpp"

namespace ksai {

	enum class VulkanDeviceFeatureSet
	{
		Default,
		Minimal,
		Extensive
	};

	class VulkanDevice
	{
	public:
		VulkanDevice(const VulkanPhysicalDevice& inPhysicalDevice, const VulkanQueueContext& inQueueContext, VulkanDeviceFeatureSet inFeatureSet = VulkanDeviceFeatureSet::Extensive);
		~VulkanDevice() {mDevice.destroy(); }
		void Wait() const { mDevice.waitIdle(); }
		GETTER& GetDeviceHandle() const { return mDevice; }
		GETTER& GetPhysicalDeviceHandle() const { return mPhysicalDevice; }
	private:
		std::vector<vk::QueueFamilyProperties> mQueueFamilyProperties;
		const vk::PhysicalDevice& mPhysicalDevice;
		vk::Device mDevice;
	};

}
