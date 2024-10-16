#include "VulkanShaderModule.hpp"

ksai::VulkanShaderModule::VulkanShaderModule(VulkanDevice &inDevice,
                                             const std::vector<uint32_t> &inSPIRV,
                                             bool inShouldDestroy) {
    Init({&inDevice, &inSPIRV, inShouldDestroy});
}

namespace ksai {
void VulkanShaderModule::Init(CreateInfo inCreateInfo) {
    mDevice   = &inCreateInfo.inDevice->GetDeviceHandle();
    mCompiler = std::make_shared<spirv_cross::Compiler>(std::ref(*inCreateInfo.inSPIRV));
    vk::ShaderModuleCreateInfo info(vk::ShaderModuleCreateFlags(), *inCreateInfo.inSPIRV);
    mResources     = mCompiler->get_shader_resources();
    mModule        = mDevice->createShaderModule(info);
    mShouldDestroy = inCreateInfo.inShouldDestroy;
    mInitialized   = true;
}

void VulkanShaderModule::Destroy() {
    if (mShouldDestroy) {
        mDevice->destroyShaderModule(mModule);
    }
}

VulkanShaderModule::~VulkanShaderModule() {
    if (mInitialized) {
        Destroy();
    }
}
} // namespace ksai