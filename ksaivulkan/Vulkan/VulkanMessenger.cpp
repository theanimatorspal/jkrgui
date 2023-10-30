#include "VulkanMessenger.hpp"
#include <iostream>
#include <sstream>
#include <vulkan/vulkan_raii.hpp>
#include <Windows.h>
using namespace ksai;

VKAPI_ATTR VkBool32 VKAPI_CALL KsaiDebugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData, void* /*pUserData*/)
{
	std::ostringstream message;
	message << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)) << ": "
		<< vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageTypes)) << ":\n";
	message << std::string("\t") << "messageIDName   = <" << pCallbackData->pMessageIdName << ">\n";
	message << std::string("\t") << "messageIdNumber = " << pCallbackData->messageIdNumber << "\n";
	message << std::string("\t") << "message         = <" << pCallbackData->pMessage << ">\n";
	if (0 < pCallbackData->queueLabelCount)
	{
		message << std::string("\t") << "Queue Labels:\n";
		for (uint32_t i = 0; i < pCallbackData->queueLabelCount; i++)
		{
			message << std::string("\t\t") << "labelName = <" << pCallbackData->pQueueLabels[i].pLabelName << ">\n";
		}
	}
	if (0 < pCallbackData->cmdBufLabelCount)
	{
		message << std::string("\t") << "CommandBuffer Labels:\n";
		for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; i++)
		{
			message << std::string("\t\t") << "labelName = <" << pCallbackData->pCmdBufLabels[i].pLabelName << ">\n";
		}
	}
	if (0 < pCallbackData->objectCount)
	{
		message << std::string("\t") << "Objects:\n";
		for (uint32_t i = 0; i < pCallbackData->objectCount; i++)
		{
			message << std::string("\t\t") << "Object " << i << "\n";
			message << std::string("\t\t\t") << "objectType   = " << vk::to_string(static_cast<vk::ObjectType>(pCallbackData->pObjects[i].objectType)) << "\n";
			message << std::string("\t\t\t") << "objectHandle = " << pCallbackData->pObjects[i].objectHandle << "\n";
			if (pCallbackData->pObjects[i].pObjectName)
			{
				message << std::string("\t\t\t") << "objectName   = <" << pCallbackData->pObjects[i].pObjectName << ">\n";
			}
		}
	}

#ifdef _WIN32
	std::cout << message.str() << std::endl;
	MessageBoxA(NULL, message.str().c_str(), "Alert", MB_ICONERROR);
#else
	std::cout << message.str() << std::endl;
#endif
	__debugbreak();
	return false;
}

PFN_vkCreateDebugUtilsMessengerEXT  pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger)
{
	return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, VkAllocationCallbacks const* pAllocator)
{
	return pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

VulkanMessenger::VulkanMessenger(const VulkanInstance& inInstance) : mInstance(inInstance.GetInstanceHandle())
{
	const auto& Instance = inInstance.GetInstanceHandle();
	pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(Instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
	if (!pfnVkCreateDebugUtilsMessengerEXT)
	{
		std::cout << "GetInstanceProcAddr: Unable to find pfnVkCreateDebugUtilsMessengerEXT function." << std::endl;
	}

	pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(Instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
	if (!pfnVkDestroyDebugUtilsMessengerEXT)
	{
		std::cout << "GetInstanceProcAddr: Unable to find pfnVkDestroyDebugUtilsMessengerEXT function." << std::endl;
		exit(1);
	}

	vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
		//| vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo 
	);
	vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

#if defined(_DEBUG)
	auto DebugUtilsMessengerCreateInfo = vk::DebugUtilsMessengerCreateInfoEXT({}, severityFlags, messageTypeFlags, &KsaiDebugMessengerCallback);
	mMessenger = Instance.createDebugUtilsMessengerEXT(DebugUtilsMessengerCreateInfo);
#endif
}

VulkanMessenger::~VulkanMessenger()
{
	mInstance.destroyDebugUtilsMessengerEXT(mMessenger);
}
