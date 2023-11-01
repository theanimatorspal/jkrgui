#include "VulkanDevice.hpp"
#include <iostream>

using namespace ksai;

VulkanDevice::VulkanDevice(const VulkanPhysicalDevice& inPhysicalDevice, const VulkanQueueContext & inQueueContext) : mPhysicalDevice(inPhysicalDevice.GetPhysicalDeviceHandle())
{
	float QueuePriority = 0.0f;
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), static_cast<uint32_t>(inQueueContext.GetGraphicsQueueFamilyIndex()), 1, &QueuePriority);

	std::vector<vk::ExtensionProperties> extensionsProperties = mPhysicalDevice.enumerateDeviceExtensionProperties();
	std::vector<vk::LayerProperties> layerProperties = mPhysicalDevice.enumerateDeviceLayerProperties();
	std::vector<char const*> deviceLayerNames;
	std::vector<char const*> deviceExtensionNames;
	deviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	vk::PhysicalDeviceFeatures Features;
	Features.fillModeNonSolid = VK_TRUE;

	auto deviceCreateInfo = vk::DeviceCreateInfo(
		vk::DeviceCreateFlags(),
		deviceQueueCreateInfo,
		{},
		deviceExtensionNames,
		&Features
	);

#ifdef USE_VARIABLE_DESCRIPTOR_INDEXING_FEATURE
	vk::PhysicalDeviceDescriptorIndexingFeatures DescriptorIndexingFeatures;
	vk::StructureChain< vk::DeviceCreateInfo, vk::PhysicalDeviceDescriptorIndexingFeatures> createInfo(
		deviceCreateInfo, DescriptorIndexingFeatures
	);
	createInfo.get<vk::PhysicalDeviceDescriptorIndexingFeatures>().runtimeDescriptorArray = VK_TRUE;
	createInfo.get<vk::PhysicalDeviceDescriptorIndexingFeatures>().descriptorBindingVariableDescriptorCount = VK_TRUE;
	createInfo.get<vk::PhysicalDeviceDescriptorIndexingFeatures>().descriptorBindingPartiallyBound = VK_TRUE;
	createInfo.get<vk::PhysicalDeviceDescriptorIndexingFeatures>().descriptorBindingUpdateUnusedWhilePending = VK_TRUE;
	mDevice = mPhysicalDevice.createDevice(createInfo.get<vk::DeviceCreateInfo>());
#else
	mDevice = mPhysicalDevice.createDevice(deviceCreateInfo);
#endif
}

VulkanDevice::~VulkanDevice()
{
	mDevice.destroy();
}

void VulkanDevice::Wait() const
{
	mDevice.waitIdle();
}

