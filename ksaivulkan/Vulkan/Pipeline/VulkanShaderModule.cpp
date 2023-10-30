#include "VulkanShaderModule.hpp"

ksai::VulkanShaderModule::VulkanShaderModule(const VulkanDevice& inDevice, const std::vector<uint32_t>& inSPIRV) 
	: mDevice(inDevice.GetDeviceHandle()), mCompiler(inSPIRV)
{
    mModule = mDevice.createShaderModule(vk::ShaderModuleCreateInfo(vk::ShaderModuleCreateFlags(), inSPIRV));
    mResources = mCompiler.get_shader_resources();
}

ksai::VulkanShaderModule::~VulkanShaderModule() {
	mDevice.destroyShaderModule(mModule);
}
