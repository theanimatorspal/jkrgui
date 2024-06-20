#include "VulkanShaderModule.hpp"

ksai::VulkanShaderModule::VulkanShaderModule(const VulkanDevice& inDevice,
                                             const std::vector<uint32_t>& inSPIRV,
                                             bool inShouldDestroy)
    : mDevice(inDevice.GetDeviceHandle()) {
    mCompiler = std::make_shared<spirv_cross::Compiler>(inSPIRV);
    vk::ShaderModuleCreateInfo info(vk::ShaderModuleCreateFlags(), inSPIRV);
    mResources     = mCompiler->get_shader_resources();
    mModule        = mDevice.createShaderModule(info);
    mShouldDestroy = inShouldDestroy;
}

ksai::VulkanShaderModule::~VulkanShaderModule() {
    if (mShouldDestroy) {
        mDevice.destroyShaderModule(mModule);
    }
}
