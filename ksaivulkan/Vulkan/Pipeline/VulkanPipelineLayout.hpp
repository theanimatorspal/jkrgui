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
    struct CreateInfo {
        const VulkanDevice *inDevice;
    };

    VulkanPipelineLayoutBase() = default;
    ~VulkanPipelineLayoutBase();
    VulkanPipelineLayoutBase(const VulkanPipelineLayoutBase &other)            = delete;
    VulkanPipelineLayoutBase &operator=(const VulkanPipelineLayoutBase &other) = delete;
    VulkanPipelineLayoutBase(VulkanPipelineLayoutBase &&other)                 = default;
    VulkanPipelineLayoutBase &operator=(VulkanPipelineLayoutBase &&other)      = default;

    void Init(CreateInfo inCreateInfo);
    void Destroy();

    VulkanPipelineLayoutBase(const VulkanDevice &inDevice) { Init({&inDevice}); }

    void FillPushConstantRanges(const spirv_cross::ShaderResources &VertexResources,
                                const spirv_cross::Compiler &inVertexComp,
                                const spirv_cross::ShaderResources &inFragmentResources,
                                const spirv_cross::Compiler &inFragmentComp,
                                std::vector<vk::PushConstantRange> &PushConstantRanges);

    public:
    GETTER &GetPipelineLayoutHandle() const { return mPipelineLayout; }

    protected:
    const vk::Device *mDevice;
    vk::PipelineLayout mPipelineLayout;
    bool mInitialized = false;
};
} // namespace ksai

namespace ksai {
template <size_t NoOfShaderModules> class VulkanPipelineLayout : public VulkanPipelineLayoutBase {
    public:
    struct CreateInfo {
        const VulkanDevice *inDevice;
        const VulkanDescriptorSetLayoutBase *inDescriptorSetLayout;
        const std::vector<VulkanShaderModule> *inModules;
    };

    VulkanPipelineLayout()                                             = default;
    ~VulkanPipelineLayout()                                            = default;
    VulkanPipelineLayout(const VulkanPipelineLayout &other)            = delete;
    VulkanPipelineLayout &operator=(const VulkanPipelineLayout &other) = delete;
    VulkanPipelineLayout(VulkanPipelineLayout &&other)                 = default;
    VulkanPipelineLayout &operator=(VulkanPipelineLayout &&other)      = default;
    operator vk::PipelineLayout() const { return mPipelineLayout; }

    VulkanPipelineLayout(const VulkanDevice &inDevice,
                         const VulkanDescriptorSetLayoutBase &inDescriptorSetLayout,
                         const std::vector<VulkanShaderModule> &inModules);

    private:
    const std::vector<VulkanShaderModule> &mModules;
};
} // namespace ksai
