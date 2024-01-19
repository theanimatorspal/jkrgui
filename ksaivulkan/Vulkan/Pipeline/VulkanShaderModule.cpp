#include "VulkanShaderModule.hpp"

ksai::VulkanShaderModule::VulkanShaderModule(const VulkanDevice& inDevice, const std::vector<uint32_t>& inSPIRV) 
	: mDevice(inDevice.GetDeviceHandle())
{
    ksai_print("ShaderModule Creation");
    mCompiler = std::make_unique<spirv_cross::Compiler>(inSPIRV);
    vk::ShaderModuleCreateInfo info(vk::ShaderModuleCreateFlags(), inSPIRV);
    mModule = mDevice.createShaderModule(info);
    ksai_print("ShaderModule Creation Finished");
    mResources = mCompiler->get_shader_resources();
    ksai_print("ShaderModule Recieved");
}

ksai::VulkanShaderModule::~VulkanShaderModule() {
	mDevice.destroyShaderModule(mModule);
}
