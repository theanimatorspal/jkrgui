#pragma once
#include "VulkanDevice.hpp"
#include <spirv_cross/spirv_cross.hpp>

namespace ksai {
enum class ShaderModuleContext { Vertex, Fragment, Geometry, Compute };

class VulkanShaderModule {
    public:
    struct CreateInfo {
        const VulkanDevice *inDevice;
        const std::vector<uint32_t> *inSPIRV;
        bool inShouldDestroy = true;
    };

    VulkanShaderModule() = default;
    ~VulkanShaderModule();
    VulkanShaderModule(const VulkanShaderModule &other)            = delete;
    VulkanShaderModule &operator=(const VulkanShaderModule &other) = delete;
    VulkanShaderModule(VulkanShaderModule &&other)                 = default;
    VulkanShaderModule &operator=(VulkanShaderModule &&other)      = default;

    void Init(CreateInfo inCreateInfo);
    void Destroy();
    VulkanShaderModule(const VulkanDevice &inDevice,
                       const std::vector<uint32_t> &inSPIRV,
                       bool inShouldDestroy = true);
    GETTER &GetShaderModuleHandle() const { return mModule; }
    GETTER &GetShaderResourcesHandle() const { return mResources; }
    GETTER &GetShaderResourcesCompilerHandle() const { return *mCompiler; }

    private:
    const vk::Device *mDevice;
    std::shared_ptr<spirv_cross::Compiler> mCompiler;
    spirv_cross::ShaderResources mResources;
    vk::ShaderModule mModule;
    bool mShouldDestroy = true;
    bool mInitialized   = false;
};
} // namespace ksai
