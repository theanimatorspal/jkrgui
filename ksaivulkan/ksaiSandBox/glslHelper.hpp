#pragma once
#include "glslang/Public/ShaderLang.h"
#include "glslang/SPIRV/GlslangToSpv.h"
#include <vulkan/vulkan.hpp>

struct SpirvHelper {
    static void Init() { glslang::InitializeProcess(); }
    static void Finalize() { glslang::FinalizeProcess(); }
    static void InitResources(TBuiltInResource& Resources);
    static EShLanguage FindLanguage(const vk::ShaderStageFlagBits shader_type);
    static bool GLSLtoSPV(const vk::ShaderStageFlagBits shader_type,
                          const char* pshader,
                          std::vector<unsigned int>& spirv);
};