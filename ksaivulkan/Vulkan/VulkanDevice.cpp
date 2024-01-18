#include "VulkanDevice.hpp"
#include "vulkan/vulkan_handles.hpp"
#include <iostream>
#ifdef __APPLE__
#include <Vulkan/vulkan_beta.h>
#endif

using namespace ksai;

VulkanDevice::VulkanDevice(const VulkanPhysicalDevice& inPhysicalDevice, const VulkanQueueContext& inQueueContext)
    : mPhysicalDevice(inPhysicalDevice.GetPhysicalDeviceHandle())
{
    float QueuePriority = 0.0f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), static_cast<ui>(inQueueContext.GetGraphicsQueueFamilyIndex()), 1, &QueuePriority);

    v<vk::ExtensionProperties> extensionsProperties = mPhysicalDevice.enumerateDeviceExtensionProperties();
    v<vk::LayerProperties> layerProperties = mPhysicalDevice.enumerateDeviceLayerProperties();
    v<char const*> deviceLayerNames;
    v<char const*> deviceExtensionNames;
    deviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

#ifdef __APPLE__
    deviceExtensionNames.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

    auto physicaldevicefeatures = mPhysicalDevice.getFeatures();

    vk::PhysicalDeviceFeatures Features;
    if (physicaldevicefeatures.fillModeNonSolid == VK_FALSE)
    {
        ksai_print("Fill Mode Non Solid is not supported in This Device");
    } else {
        Features.fillModeNonSolid = VK_TRUE;
    }

    auto deviceCreateInfo = vk::DeviceCreateInfo(
        vk::DeviceCreateFlags(),
        deviceQueueCreateInfo,
        {},
        deviceExtensionNames,
        &Features);

#ifdef USE_VARIABLE_DESCRIPTOR_INDEXING_FEATURE
#ifdef __APPLE__
    vk::PhysicalDeviceDescriptorIndexingFeaturesEXT DescriptorIndexingFeatures;
    DescriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;
    DescriptorIndexingFeatures.descriptorBindingVariableDescriptorCount = VK_TRUE;
    DescriptorIndexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
    DescriptorIndexingFeatures.descriptorBindingUpdateUnusedWhilePending = VK_TRUE;
    DescriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;

    vk::StructureChain<vk::DeviceCreateInfo, vk::PhysicalDeviceDescriptorIndexingFeaturesEXT>
        createInfo(deviceCreateInfo, DescriptorIndexingFeatures);

    mDevice = mPhysicalDevice.createDevice(createInfo.get<vk::DeviceCreateInfo>());
#elif ANDROID
    ksai_print("Started Vulkan Device Creation");
    mDevice = mPhysicalDevice.createDevice(deviceCreateInfo);
    ksai_print("Started Vulkan Device Creation Finished");
#else
    vk::PhysicalDeviceDescriptorIndexingFeatures DescriptorIndexingFeatures;
    DescriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;
    DescriptorIndexingFeatures.descriptorBindingVariableDescriptorCount = VK_TRUE;
    DescriptorIndexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
    DescriptorIndexingFeatures.descriptorBindingUpdateUnusedWhilePending = VK_TRUE;
    DescriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;

    vk::StructureChain<vk::DeviceCreateInfo, vk::PhysicalDeviceDescriptorIndexingFeatures> createInfo(
        deviceCreateInfo, DescriptorIndexingFeatures);

    mDevice = mPhysicalDevice.createDevice(createInfo.get<vk::DeviceCreateInfo>());

#endif

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
