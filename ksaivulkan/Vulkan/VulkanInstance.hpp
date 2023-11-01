#pragma once
#include "Config.hpp"
#include <vulkan/vulkan.hpp>
#ifndef GETTER
#define GETTER inline auto
#endif
#define CONSTANT constexpr auto
namespace ksai {

	class VulkanInstance
	{
	public:
		VulkanInstance();
		~VulkanInstance();
		inline constexpr const auto& GetInstanceHandle() const { return mInstance; }
	private:
		vk::Instance mInstance;
		std::vector <char const*> mInstanceLayerNames;
		std::vector <char const*> mInstanceExtensionNames;
	};

}
