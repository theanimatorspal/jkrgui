#pragma once
#include "VulkanDescriptorSetLayout.hpp"

namespace ksai {
enum class PipelineContext {
    Default,
    DefaultSingleSampled,
    DefaultWithGeometry,
    Skybox,
    Compute,
    Shadow
};
}

namespace ksai {
class VulkanPipelineLayoutBase {
    public:
    VulkanPipelineLayoutBase(const VulkanDevice& inDevice) : mDevice(inDevice.GetDeviceHandle()) {}
    ~VulkanPipelineLayoutBase() { mDevice.destroyPipelineLayout(mPipelineLayout); }

    void FillPushConstantRanges(const spirv_cross::ShaderResources& VertexResources,
                                const spirv_cross::Compiler& inVertexComp,
                                const spirv_cross::ShaderResources& inFragmentResources,
                                const spirv_cross::Compiler& inFragmentComp,
                                std::vector<vk::PushConstantRange>& PushConstantRanges);

    public:
    GETTER& GetPipelineLayoutHandle() const { return mPipelineLayout; }

    protected:
    const vk::Device& mDevice;
    vk::PipelineLayout mPipelineLayout;
};
} // namespace ksai

namespace ksai {
template <size_t NoOfShaderModules> class VulkanPipelineLayout : public VulkanPipelineLayoutBase {
    public:
    VulkanPipelineLayout(const VulkanDevice& inDevice,
                         const VulkanDescriptorSetLayoutBase& inDescriptorSetLayout,
                         const std::vector<VulkanShaderModule>& inModules);
    ~VulkanPipelineLayout() = default;

    private:
    const std::vector<VulkanShaderModule>& mModules;
};
} // namespace ksai
