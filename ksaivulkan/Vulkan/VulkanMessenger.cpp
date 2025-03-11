#include "VulkanMessenger.hpp"
#ifdef WIN32
#include <Windows.h>
#elif ANDROID
#include <SDL.h>
#endif

#include <iostream>
#include <sstream>
#include <vulkan/vulkan_raii.hpp>
using namespace ksai;

VKAPI_ATTR VkBool32 VKAPI_CALL
KsaiDebugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                           VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                           VkDebugUtilsMessengerCallbackDataEXT const *pCallbackData,
                           void * /*pUserData*/) {
    std::ostringstream message;
    message << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity))
            << ": " << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageTypes))
            << ":\n";
    message << "\tmessageIDName   = <" << pCallbackData->pMessageIdName << ">\n";
    message << "\tmessageIdNumber = " << pCallbackData->messageIdNumber << "\n";

    std::string callbackDataMessage = pCallbackData->pMessage;
    std::replace_if(
         callbackDataMessage.begin(),
         callbackDataMessage.end(),
         [](char c) { return c == ',' || c == ';' || c == '|'; },
         '\n');

    message << "\tmessage         = (\n\n" << callbackDataMessage << "\n)\n\n";

    if (pCallbackData->queueLabelCount > 0) {
        message << "\tQueue Labels:\n";
        for (uint32_t i = 0; i < pCallbackData->queueLabelCount; i++) {
            message << "\t\tlabelName = <" << pCallbackData->pQueueLabels[i].pLabelName << ">\n";
        }
    }

    if (pCallbackData->cmdBufLabelCount > 0) {
        message << "\tCommandBuffer Labels:\n";
        for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; i++) {
            message << "\t\tlabelName = <" << pCallbackData->pCmdBufLabels[i].pLabelName << ">\n";
        }
    }

    if (pCallbackData->objectCount > 0) {
        message << "\tObjects:\n";
        for (uint32_t i = 0; i < pCallbackData->objectCount; i++) {
            message << "\t\tObject " << i << "\n";
            message << "\t\t\tobjectType   = "
                    << vk::to_string(
                            static_cast<vk::ObjectType>(pCallbackData->pObjects[i].objectType))
                    << "\n";
            message << "\t\t\tobjectHandle = " << pCallbackData->pObjects[i].objectHandle << "\n";
            if (pCallbackData->pObjects[i].pObjectName) {
                message << "\t\t\tobjectName   = <" << pCallbackData->pObjects[i].pObjectName
                        << ">\n";
            }
        }
    }

    std::string messageString = message.str();

    bool isError       = messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    bool isWarning     = messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    bool isInfo        = messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    bool isPerformance = messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

#ifdef _WIN32
    if (isError) {
        MessageBoxA(NULL, messageString.c_str(), "Error", MB_ICONERROR);
    } else if (isWarning && !(isPerformance)) {
        MessageBoxA(NULL, messageString.c_str(), "Warning", MB_ICONWARNING);
    }
#endif
    Log(messageString, isInfo ? "INFO" : 
        (isError ? "ERROR" : 
            (isWarning ? "WARNING" : 
                "INFO"
            )));

    return VK_FALSE;
}

PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDebugUtilsMessengerEXT(VkInstance instance,
                               const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                               const VkAllocationCallbacks *pAllocator,
                               VkDebugUtilsMessengerEXT *pMessenger) {
    return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyDebugUtilsMessengerEXT(VkInstance instance,
                                VkDebugUtilsMessengerEXT messenger,
                                VkAllocationCallbacks const *pAllocator) {
    return pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

VulkanMessenger::VulkanMessenger(VulkanInstance &inInstance)
    : mInstance(inInstance.GetInstanceHandle()) {
#if defined(_DEBUG)
    const auto &Instance              = inInstance.GetInstanceHandle();
    pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
         Instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
    if (!pfnVkCreateDebugUtilsMessengerEXT) {
        Log("GetInstanceProcAddr: Unable to find pfnVkCreateDebugUtilsMessengerEXT function", "ERROR");
    }

    pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
         Instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
    if (!pfnVkDestroyDebugUtilsMessengerEXT) {
        Log("GetInstanceProcAddr: Unable to find pfnVkDestroyDebugUtilsMessengerEXT function", "ERROR");
    }

    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
         vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
         vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
         vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo);
    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
         vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
         vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
         vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
         vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding);

    auto DebugUtilsMessengerCreateInfo = vk::DebugUtilsMessengerCreateInfoEXT(
         {}, severityFlags, messageTypeFlags, &KsaiDebugMessengerCallback);
#if !defined(ANDROID) // My Android device Doesn't Support this
    mMessenger = Instance.createDebugUtilsMessengerEXT(DebugUtilsMessengerCreateInfo);
#endif
#endif
}

VulkanMessenger::~VulkanMessenger() {
#if defined(_DEBUG)
#if !defined(ANDROID)
    mInstance.destroyDebugUtilsMessengerEXT(mMessenger);
#endif
#endif
}
