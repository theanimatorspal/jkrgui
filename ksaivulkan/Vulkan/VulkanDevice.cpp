#include "VulkanDevice.hpp"
#include "vulkan/vulkan_handles.hpp"
#include <iostream>
#ifdef __APPLE__
#include <Vulkan/vulkan_beta.h>
#endif

using namespace ksai;

VulkanDevice::VulkanDevice(const VulkanPhysicalDevice &inPhysicalDevice,
                           const VulkanQueueContext &inQueueContext,
                           VulkanDeviceFeatureSet inFeatureSet)
    : mPhysicalDevice(&inPhysicalDevice.GetPhysicalDeviceHandle()) {
    Init({&inPhysicalDevice, &inQueueContext, inFeatureSet});
}

void VulkanDevice::Init(CreateInfo inCreateInfo) {
    mVulkanPhysicalDevice = inCreateInfo.mPhysicalDevice;
    mPhysicalDevice       = &inCreateInfo.mPhysicalDevice->GetPhysicalDeviceHandle();
    auto &inQueueContext  = *inCreateInfo.mQueueContext;
    auto inFeatureSet     = inCreateInfo.mFeatureSet;

    float QueuePriority   = 0.0f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo(
         vk::DeviceQueueCreateFlags(),
         static_cast<ui>(inQueueContext.GetGraphicsQueueFamilyIndex()),
         1,
         &QueuePriority);
    v<vk::ExtensionProperties> extensionsProperties =
         mPhysicalDevice->enumerateDeviceExtensionProperties();
    v<vk::LayerProperties> layerProperties = mPhysicalDevice->enumerateDeviceLayerProperties();
    v<char const *> deviceLayerNames;
    v<char const *> deviceExtensionNames;
    deviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
#ifdef __APPLE__
    deviceExtensionNames.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif
    auto physicaldevicefeatures = mPhysicalDevice->getFeatures();

    switch (inFeatureSet) {
        case VulkanDeviceFeatureSet::Default: {
        } break;

        case VulkanDeviceFeatureSet::Minimal: {
            auto deviceCreateInfo = vk::DeviceCreateInfo(
                 vk::DeviceCreateFlags(), deviceQueueCreateInfo, {}, deviceExtensionNames);

            mDevice = mPhysicalDevice->createDevice(deviceCreateInfo);
        } break;
        case VulkanDeviceFeatureSet::Extensive: {
            vk::PhysicalDeviceFeatures Features;
            //  TODO Make this better
#ifndef ANDROID
            Features.fillModeNonSolid = VK_TRUE;
#endif

            auto deviceCreateInfo = vk::DeviceCreateInfo(vk::DeviceCreateFlags(),
                                                         deviceQueueCreateInfo,
                                                         {},
                                                         deviceExtensionNames,
                                                         &Features);
#ifdef __APPLE__
            vk::PhysicalDeviceDescriptorIndexingFeaturesEXT DescriptorIndexingFeatures;
            DescriptorIndexingFeatures.runtimeDescriptorArray                    = VK_TRUE;
            DescriptorIndexingFeatures.descriptorBindingVariableDescriptorCount  = VK_TRUE;
            DescriptorIndexingFeatures.descriptorBindingPartiallyBound           = VK_TRUE;
            DescriptorIndexingFeatures.descriptorBindingUpdateUnusedWhilePending = VK_TRUE;
            DescriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;

            vk::StructureChain<vk::DeviceCreateInfo,
                               vk::PhysicalDeviceDescriptorIndexingFeaturesEXT>
                 createInfo(deviceCreateInfo, DescriptorIndexingFeatures);

            mDevice = mPhysicalDevice->createDevice(createInfo.get<vk::DeviceCreateInfo>());
#elif ANDROID
            mDevice = mPhysicalDevice->createDevice(deviceCreateInfo);
#else
            vk::PhysicalDeviceDescriptorIndexingFeatures DescriptorIndexingFeatures;
            DescriptorIndexingFeatures.runtimeDescriptorArray                    = VK_TRUE;
            DescriptorIndexingFeatures.descriptorBindingVariableDescriptorCount  = VK_TRUE;
            DescriptorIndexingFeatures.descriptorBindingPartiallyBound           = VK_TRUE;
            DescriptorIndexingFeatures.descriptorBindingUpdateUnusedWhilePending = VK_TRUE;
            DescriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;

            vk::StructureChain<vk::DeviceCreateInfo, vk::PhysicalDeviceDescriptorIndexingFeatures>
                 createInfo(deviceCreateInfo, DescriptorIndexingFeatures);

            mDevice = mPhysicalDevice->createDevice(createInfo.get<vk::DeviceCreateInfo>());
#endif
        } break;
    }

    mInitialized = true;
}

void VulkanDevice::Destroy() {
    if (mDevice) {
        mDevice.destroy();
    }
    mInitialized = false;
}

void VulkanDevice::Wait() const { mDevice.waitIdle(); }

VulkanDevice::~VulkanDevice() {
    if (mInitialized) {
        Destroy();
    }
}