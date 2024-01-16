#pragma once

#include "VulkanInstance.hpp"

namespace ksai {
	enum class MemoryType
	{
		DeviceLocal,
		HostVisibleAndCoherenet,
		HostVisibleAndHostCached
	};
}

namespace ksai {

	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice(const VulkanInstance& inInstance);
		~VulkanPhysicalDevice();
		inline constexpr const auto& GetPhysicalDeviceHandle() const { return mPhysicalDevice; }
	private:
		const vk::Instance& mInstance;
		vk::PhysicalDevice mPhysicalDevice;
	};
}
