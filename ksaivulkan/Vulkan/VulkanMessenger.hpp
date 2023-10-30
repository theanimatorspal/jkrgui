#pragma once
#include "VulkanInstance.hpp"

namespace ksai {
	class VulkanMessenger
	{
	public:
		VulkanMessenger(const VulkanInstance& inInstance);
		~VulkanMessenger();
	private:
		const vk::Instance& mInstance;
		vk::DebugUtilsMessengerEXT mMessenger;
	};

}
