#include "VulkanInstance.hpp"
#include "vulkan/vulkan_core.h"
#ifdef __APPLE__
#include "vulkan/vulkan_beta.h"
#endif
#ifdef WIN32
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <Windows.h>
#elif ANDROID
#include <SDL.h>
#include <SDL_vulkan.h>
#include <android/log.h>
#endif
#include <iostream>

using namespace ksai;

static s AppName = "JkrEngine";
static s EngineName = "JkrVulkanEngine";
static bool hasLayers(std::span<char const*> const& layers,
    std::span<vk::LayerProperties> const& properties)
{
    return std::all_of(layers.begin(), layers.end(), [&properties](char const* name) {
        return std::any_of(properties.begin(), properties.end(),
            [&name](vk::LayerProperties const& property) {
                ksai_print("LEFT:", property.layerName);
                ksai_print("RIGHT:", name);
                return strcmp(property.layerName, name) == 0;
            });
    });
}

// Yelle kaam garya xaina hai, beware
static bool hasExtensions(std::span<char const*> const& instanceExtensionNames,
    std::span<vk::ExtensionProperties> ExtensionProperties)
{
    auto PropertyIterator = std::find_if(
        ExtensionProperties.begin(), ExtensionProperties.end(), [&instanceExtensionNames](vk::ExtensionProperties const& ep) {
            return std::any_of(instanceExtensionNames.begin(), instanceExtensionNames.end(),
                [&ep](char const* name) {
                    return strcmp(ep.extensionName, name) == 0;
                });
        });
    return !(PropertyIterator == ExtensionProperties.end());
}

VulkanInstance::VulkanInstance()
{
    auto instanceLayerProperties = vk::enumerateInstanceLayerProperties();
#if defined(_DEBUG)
    mInstanceLayerNames.push_back("VK_LAYER_KHRONOS_validation");
#endif

#ifdef __APPLE__
    // mInstanceExtensionNames.push_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
    mInstanceExtensionNames.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

    if (not hasLayers(mInstanceLayerNames, instanceLayerProperties)) {
        std::cout << "Set the Environment VK_LAYER_PATH to point to the location your layers"
                  << std::endl;
        ksai_print("Set the Environment VK_LAYER_PATH to pont to the locatioon, layer not found");
        exit(1);
    }

    if (SDL_Init (SDL_INIT_EVERYTHING) == -1)
    {
        ksai_print("SDL returned error");
        ksai_print("Couldn't INitialize SDL: %s", SDL_GetError());
    }
    SDL_Window* Window = SDL_CreateWindow("Test", 0, 0, 100, 100, SDL_WINDOW_HIDDEN | SDL_WINDOW_VULKAN);
    auto ExtensionProperties = vk::enumerateInstanceExtensionProperties();
    ui pCount = 0;
    const char** extensions;

    SDL_Vulkan_GetInstanceExtensions(Window, &pCount, NULL);
    v<const char*> extVec(pCount);
    extensions = extVec.data();
    SDL_Vulkan_GetInstanceExtensions(Window, &pCount, extensions);
    SDL_DestroyWindow(Window);

    for (int i = 0; i < pCount; i++) {
        mInstanceExtensionNames.push_back((char const*)extensions[i]);
        ksai_print(extensions[i]);
    }

#if defined(_DEBUG)
    mInstanceExtensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    mInstanceExtensionNames.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
#endif

#ifdef USE_VULKAN_1_2
    auto ApplicationInfo = vk::ApplicationInfo("JkrGUI", 1, "JkrEngine", 1, VK_API_VERSION_1_2);
#endif

#ifdef USE_VULKAN_1_3
    auto ApplicationInfo = vk::ApplicationInfo("JkrGUI", 1, "JkrEngine", 1, VK_API_VERSION_1_3);
#endif

#ifdef USE_VULKAN_1_1
    auto ApplicationInfo = vk::ApplicationInfo("JkrGUI", 1, "JkrEngine", 1, VK_API_VERSION_1_1);
#endif
    if (!hasExtensions(mInstanceExtensionNames, ExtensionProperties)) {
        std::cout << "Something Went Very Wrong : Extension not found" << std::endl;
        ksai_print("Extension Not Found");
    }

#ifdef __APPLE__
    auto InstanceCreateInfo
        = vk::InstanceCreateInfo(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR,
            &ApplicationInfo,
            mInstanceLayerNames,
            mInstanceExtensionNames);
#else
    auto InstanceCreateInfo = vk::InstanceCreateInfo({},
        &ApplicationInfo,
        mInstanceLayerNames,
        mInstanceExtensionNames);
#endif

#ifdef USE_VULKAN_1_3
#if defined(_DEBUG)
    v<vk::ValidationFeatureEnableEXT> EnabledFeaturesValidaiton = {
        vk::ValidationFeatureEnableEXT::eSynchronizationValidation
    };
    auto ValidationFeaturesInfo = vk::ValidationFeaturesEXT()
                                      .setEnabledValidationFeatures(EnabledFeaturesValidaiton);
    vk::StructureChain<vk::InstanceCreateInfo, vk::ValidationFeaturesEXT> InstanceCreateInfo_WithValidationFeatures(InstanceCreateInfo, ValidationFeaturesInfo);
    mInstance = vk::createInstance(InstanceCreateInfo_WithValidationFeatures.get<vk::InstanceCreateInfo>());
#else
    mInstance = vk::createInstance(InstanceCreateInfo);
#endif

#else
    ksai_print("Instance print");
    mInstance = vk::createInstance(InstanceCreateInfo);
    ksai_print("Instance print created");
#endif
}

VulkanInstance::~VulkanInstance()
{
    mInstance.destroy();
    SDL_Quit();
}
