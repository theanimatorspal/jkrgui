#pragma once
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <climits>
#include <string>
#include <variant>
#include <vector>
#include <memory>
#include <iostream>
#include <cassert>
#include <sstream>
#include <optional>

#ifdef __APPLE__
#define USE_VULKAN_1_2
#define VK_VERSION_1_2
#elif ANDROID
#define USE_VULKAN_1_1
#define VK_VERSION_1_1
#undef VK_VERSION_1_3
#undef VK_VERSION_1_2
#define VMA_VULKAN_VERSION 1001000 
#else
#define USE_VULKAN_1_3
#define VK_VERSION_1_3
#endif

#define USE_VARIABLE_DESCRIPTOR_INDEXING_FEATURE

#ifndef GETTER
#define GETTER inline auto
#endif
#include "../ksai_config.hpp"
