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
//#define USE_VULKAN_1_1
//#define USE_VULKAN_1_2
#define USE_VULKAN_1_3

#define USE_VARIABLE_DESCRIPTOR_INDEXING_FEATURE

#ifndef GETTER
#define GETTER inline auto
#endif
#include "../ksai_config.hpp"
