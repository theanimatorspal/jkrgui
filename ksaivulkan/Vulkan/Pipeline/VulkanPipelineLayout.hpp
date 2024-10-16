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
        VulkanDevice *inDevice;
    };

    VulkanPipelineLayoutBase() = default;
    ~VulkanPipelineLayoutBase();
    VulkanPipelineLayoutBase(VulkanPipelineLayoutBase &other)             = delete;
    VulkanPipelineLayoutBase &operator=(VulkanPipelineLayoutBase &other)  = delete;
    VulkanPipelineLayoutBase(VulkanPipelineLayoutBase &&other)            = default;
    VulkanPipelineLayoutBase &operator=(VulkanPipelineLayoutBase &&other) = default;

    void Init(CreateInfo inCreateInfo);
    void Destroy();

    VulkanPipelineLayoutBase(VulkanDevice &inDevice) { Init({&inDevice}); }

    void FillPushConstantRanges(const spirv_cross::ShaderResources &VertexResources,
                                const spirv_cross::Compiler &inVertexComp,
                                const spirv_cross::ShaderResources &inFragmentResources,
                                const spirv_cross::Compiler &inFragmentComp,
                                std::vector<vk::PushConstantRange> &PushConstantRanges);

    public:
    GETTER &GetPipelineLayoutHandle() const { return mPipelineLayout; }

    protected:
    vk::Device *mDevice;
    vk::PipelineLayout mPipelineLayout;
    bool mInitialized = false;
};
} // namespace ksai

namespace ksai {
template <size_t NoOfShaderModules> class VulkanPipelineLayout : public VulkanPipelineLayoutBase {
    public:
    struct CreateInfo {
        VulkanDevice *inDevice;
        VulkanDescriptorSetLayoutBase *inDescriptorSetLayout;
        const std::vector<VulkanShaderModule> *inModules;
    };

    VulkanPipelineLayout()                                        = default;
    ~VulkanPipelineLayout()                                       = default;
    VulkanPipelineLayout(VulkanPipelineLayout &other)             = delete;
    VulkanPipelineLayout &operator=(VulkanPipelineLayout &other)  = delete;
    VulkanPipelineLayout(VulkanPipelineLayout &&other)            = default;
    VulkanPipelineLayout &operator=(VulkanPipelineLayout &&other) = default;
    operator vk::PipelineLayout() const { return mPipelineLayout; }

    VulkanPipelineLayout(VulkanDevice &inDevice,
                         VulkanDescriptorSetLayoutBase &inDescriptorSetLayout,
                         const std::vector<VulkanShaderModule> &inModules);

    private:
    const std::vector<VulkanShaderModule> &mModules;
};
} // namespace ksai
