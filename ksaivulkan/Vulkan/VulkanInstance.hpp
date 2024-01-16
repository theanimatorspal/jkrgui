#pragma once
#include "Config.hpp"
#include <vulkan/vulkan.hpp>
#define CONSTANT constexpr auto
namespace ksai {

	class VulkanInstance
	{
	public:
		VulkanInstance();
		~VulkanInstance();
		GETTER& GetInstanceHandle() const { return mInstance; }
	private:
		vk::Instance mInstance;
		v <char const*> mInstanceLayerNames;
		v <char const*> mInstanceExtensionNames;
	};

}
