#pragma once
#include "glslang/Public/ShaderLang.h"
#include "glslang/SPIRV/GlslangToSpv.h"
#include <vulkan/vulkan.hpp>

namespace SpirvHelper {
void Init();
void Finalize();
void InitResources(TBuiltInResource &Resources);
EShLanguage FindLanguage(const vk::ShaderStageFlagBits shader_type);
bool GLSLtoSPV(const vk::ShaderStageFlagBits shader_type,
               const char *pshader,
               std::vector<unsigned int> &spirv);
}; // namespace SpirvHelper