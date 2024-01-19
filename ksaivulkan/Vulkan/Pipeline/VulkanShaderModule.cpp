#include "VulkanShaderModule.hpp"

ksai::VulkanShaderModule::VulkanShaderModule(const VulkanDevice& inDevice, const std::vector<uint32_t>& inSPIRV) 
	: mDevice(inDevice.GetDeviceHandle())
{
    mCompiler = std::make_unique<spirv_cross::Compiler>(inSPIRV);
    vk::ShaderModuleCreateInfo info(vk::ShaderModuleCreateFlags(), inSPIRV);
    mModule = mDevice.createShaderModule(info);
    mResources = mCompiler->get_shader_resources();
}

ksai::VulkanShaderModule::~VulkanShaderModule() {
	mDevice.destroyShaderModule(mModule);
}
