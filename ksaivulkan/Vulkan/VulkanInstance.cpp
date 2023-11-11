#include "VulkanInstance.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <iostream>
#include <sstream>
#include <Windows.h>

using namespace ksai;

static std::string AppName = "JkrEngine";
static std::string EngineName = "JkrVulkanEngine";
static bool checkLayers ( std::vector<char const*> const& layers, std::vector<vk::LayerProperties> const& properties )
{
	return std::all_of ( layers.begin ( ), layers.end ( ), [&properties]( char const* name )
								{
									return std::any_of ( properties.begin ( ), properties.end ( ),
									[&name]( vk::LayerProperties const& property ) {
									return strcmp ( property.layerName, name ) == 0;
								} );
								} );
}

static bool checkExtensions ( std::vector<char const*> const& instanceExtensionNames, std::vector<vk::ExtensionProperties> ExtensionProperties )
{
	auto PropertyIterator = std::find_if (
		ExtensionProperties.begin ( ), ExtensionProperties.end ( ), [&instanceExtensionNames]( vk::ExtensionProperties const& ep )
		{
			return std::any_of ( instanceExtensionNames.begin ( ), instanceExtensionNames.end ( ),
			[&ep]( char const* name )
		{
			return strcmp ( ep.extensionName, name );
		}
	);
		}
	);
	return !(PropertyIterator == ExtensionProperties.end ( ));
}

VulkanInstance::VulkanInstance ( )
{
	auto instanceLayerProperties = vk::enumerateInstanceLayerProperties ( );
#if defined(_DEBUG)
	mInstanceLayerNames.push_back ( "VK_LAYER_KHRONOS_validation" );
#endif

	if (!checkLayers ( mInstanceLayerNames, instanceLayerProperties ))
	{
		std::cout << "Set the Environment VK_LAYER_PATH to point to the location your layers" << std::endl;
		exit ( 1 );
	}

	SDL_Window* Window = SDL_CreateWindow ( "Test", 0, 0, 1, 1, SDL_WINDOW_HIDDEN | SDL_WINDOW_VULKAN );
	auto ExtensionProperties = vk::enumerateInstanceExtensionProperties ( );
	uint32_t pCount = 0;
	const char** extensions;
	SDL_Vulkan_GetInstanceExtensions ( Window, &pCount, NULL );
	std::vector<const char*> extVec ( pCount );
	extensions = extVec.data ( );
	SDL_Vulkan_GetInstanceExtensions ( Window, &pCount, extensions );
	SDL_DestroyWindow ( Window );

	for (int i = 0; i < pCount; i++)
	{
		mInstanceExtensionNames.push_back ( (char const*)extensions[i] );
	}

#if defined(_DEBUG)
	mInstanceExtensionNames.push_back ( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
	mInstanceExtensionNames.push_back ( VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME );
#endif

	if (!checkExtensions ( mInstanceExtensionNames, ExtensionProperties ))
	{
		std::cout << "Something Went Very Wrong : Extension not found" << std::endl;
	}

#ifdef USE_VULKAN_1_2
	auto ApplicationInfo = vk::ApplicationInfo ( "JkrGUI", 1, "JkrEngine", 1, VK_API_VERSION_1_2 );
#endif
#ifdef USE_VULKAN_1_3
	auto ApplicationInfo = vk::ApplicationInfo ( "JkrGUI", 1, "JkrEngine", 1, VK_API_VERSION_1_3 );
#endif
#ifdef USE_VULKAN_1_1
	auto ApplicationInfo = vk::ApplicationInfo ( "JkrGUI", 1, "JkrEngine", 1, VK_API_VERSION_1_1 );
#endif


	auto InstanceCreateInfo = vk::InstanceCreateInfo ( {}, &ApplicationInfo, mInstanceLayerNames, mInstanceExtensionNames );
#ifdef USE_VULKAN_1_3
	std::vector<vk::ValidationFeatureEnableEXT> EnabledFeaturesValidaiton = {
		vk::ValidationFeatureEnableEXT::eGpuAssisted,
		vk::ValidationFeatureEnableEXT::eGpuAssistedReserveBindingSlot,
		vk::ValidationFeatureEnableEXT::eSynchronizationValidation ,
//		vk::ValidationFeatureEnableEXT::eBestPractices,
	};
	auto ValidationFeaturesInfo = vk::ValidationFeaturesEXT ( )
		.setEnabledValidationFeatures ( EnabledFeaturesValidaiton );
	vk::StructureChain<vk::InstanceCreateInfo, vk::ValidationFeaturesEXT> InstanceCreateInfo_WithValidationFeatures(InstanceCreateInfo, ValidationFeaturesInfo);
	mInstance = vk::createInstance ( InstanceCreateInfo_WithValidationFeatures.get<vk::InstanceCreateInfo> ( ) );
#else
	mInstance = vk::createInstance ( InstanceCreateInfo );
#endif

}

VulkanInstance::~VulkanInstance ( )
{
	mInstance.destroy ( );
}
