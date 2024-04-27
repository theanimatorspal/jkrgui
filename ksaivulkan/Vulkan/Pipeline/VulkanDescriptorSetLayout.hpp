#pragma once
#include "VulkanShaderModule.hpp"

namespace ksai {

class VulkanDescriptorSetLayoutBase {
    public:
    VulkanDescriptorSetLayoutBase(const VulkanDevice& inDevice);
    ~VulkanDescriptorSetLayoutBase();
    void FillDescriptorLayoutBindings(
         const spirv_cross::ShaderResources& inResources,
         const spirv_cross::Compiler& inGlslComp,
         std::vector<vk::DescriptorSetLayoutBinding>& DescriptorSetLayoutBindings,
         int inSet = 0);
    void FillDescriptorLayoutBindings(
         ShaderModuleContext Context,
         const spirv_cross::ShaderResources& inVertexResources,
         const spirv_cross::Compiler& inGlslCompVertex,
         std::vector<vk::DescriptorSetLayoutBinding>& DescriptorSetLayoutBindings,
         int inSet = 0);

    public:
    GETTER& GetDescriptorLayoutHandle() const { return mDescriptorSetLayouts; }

    protected:
    void
    CheckAndPushBindings(std::vector<vk::DescriptorSetLayoutBinding>& DescriptorSetLayoutBindings,
                         vk::DescriptorSetLayoutBinding inBinding);

    protected:
    const vk::Device& mDevice;
    v<vk::DescriptorSetLayout> mDescriptorSetLayouts;
};

} // namespace ksai

namespace ksai {
template <size_t NoOfShaderModules, ShaderModuleContext... inContexts>
class VulkanDescriptorSetLayout : public VulkanDescriptorSetLayoutBase {
    public:
    VulkanDescriptorSetLayout(const VulkanDevice& inDevice,
                              const std::vector<VulkanShaderModule>& inModules);
    VulkanDescriptorSetLayout(const VulkanDevice& inDevice,
                              const std::vector<VulkanShaderModule>& inModules,
                              uint32_t inVariableDescriptorsMaxCount);
    ~VulkanDescriptorSetLayout() = default;

    private:
    const std::vector<VulkanShaderModule>& mModules;
};
} // namespace ksai
