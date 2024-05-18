#include "TestUtils.hpp"

using namespace Jkr;

ShaderCompiler::ShaderCompiler(const std::string& inVertexShaderString,
                               const std::string& inFragmentShaderString,
                               std::vector<uint32_t>& outVertexShaderModule,
                               std::vector<uint32_t>& outFragmentShaderModule) {
    bool success = SpirvHelper::GLSLtoSPV(
         vk::ShaderStageFlagBits::eVertex, inVertexShaderString.c_str(), outVertexShaderModule);
    if (!success) std::cout << "VertexShader Failed=============================\n";
    success = SpirvHelper::GLSLtoSPV(vk::ShaderStageFlagBits::eFragment,
                                     inFragmentShaderString.c_str(),
                                     outFragmentShaderModule);
    if (!success) std::cout << "FragmentShader Failed=============================\n";
}

ShaderCompiler::ShaderCompiler(const std::string& inComputeShaderString,
                               std::vector<uint32_t>& outComputeShaderModule) {
    bool success = SpirvHelper::GLSLtoSPV(
         vk::ShaderStageFlagBits::eCompute, inComputeShaderString.c_str(), outComputeShaderModule);
    if (!success) std::cout << "ComputeShader Failed=============================\n";
}