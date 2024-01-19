#pragma once
#include "VulkanShaderModule.hpp"
#include <initializer_list>
#include <tuple>

namespace ksai {

class VulkanDescriptorSetLayoutBase {
public:
    VulkanDescriptorSetLayoutBase(const VulkanDevice& inDevice);
    ~VulkanDescriptorSetLayoutBase();
    void FillDescriptorLayoutBindings(const spirv_cross::ShaderResources& inResources, const spirv_cross::Compiler& inGlslComp, std::vector<vk::DescriptorSetLayoutBinding>& DescriptorSetLayoutBindings);
    template <ShaderModuleContext Context>
    void FillDescriptorLayoutBindings(const spirv_cross::ShaderResources& inVertexResources, const spirv_cross::Compiler& inGlslCompVertex, std::vector<vk::DescriptorSetLayoutBinding>& DescriptorSetLayoutBindings);

public:
    GETTER& GetDescriptorLayoutHandle() const { return mDescriptorSetLayout; }

protected:
    void CheckAndPushBindings(std::vector<vk::DescriptorSetLayoutBinding>& DescriptorSetLayoutBindings, vk::DescriptorSetLayoutBinding inBinding);

protected:
    const vk::Device& mDevice;
    vk::DescriptorSetLayout mDescriptorSetLayout;
};

}

namespace ksai {
template <size_t NoOfShaderModules, ShaderModuleContext... inContexts>
class VulkanDescriptorSetLayout : public VulkanDescriptorSetLayoutBase {
public:
    VulkanDescriptorSetLayout(const VulkanDevice& inDevice, const std::array<VulkanShaderModule, NoOfShaderModules>& inModules);
    VulkanDescriptorSetLayout(const VulkanDevice& inDevice, const std::array<VulkanShaderModule, NoOfShaderModules>& inModules, uint32_t inVariableDescriptorsMaxCount);
    ~VulkanDescriptorSetLayout() = default;

private:
    const std::array<VulkanShaderModule, NoOfShaderModules>& mModules;
};
}

namespace ksai {
template <ShaderModuleContext Context>
inline void VulkanDescriptorSetLayoutBase::FillDescriptorLayoutBindings(const spirv_cross::ShaderResources& inResources, const spirv_cross::Compiler& inGlslCompiler, std::vector<vk::DescriptorSetLayoutBinding>& inDescriptorSetLayoutBindings)
{
    ksai_print("Starting Des set layout binding");
    auto ShaderStageFlag = vk::ShaderStageFlagBits::eVertex;
    if (Context == ShaderModuleContext::Vertex)
        ShaderStageFlag = vk::ShaderStageFlagBits::eVertex;
    else if (Context == ShaderModuleContext::Fragment)
        ShaderStageFlag = vk::ShaderStageFlagBits::eFragment;
    else if (Context == ShaderModuleContext::Compute)
        ShaderStageFlag = vk::ShaderStageFlagBits::eCompute;

    ksai_print("Starting Des set layout binding 1");
    for (auto& u : inResources.uniform_buffers) {
        uint32_t set = inGlslCompiler.get_decoration(u.id, spv::DecorationDescriptorSet);
        uint32_t binding = inGlslCompiler.get_decoration(u.id, spv::DecorationBinding);
        auto dbinding = vk::DescriptorSetLayoutBinding(binding, vk::DescriptorType::eUniformBuffer, 1, ShaderStageFlag, nullptr);
        CheckAndPushBindings(inDescriptorSetLayoutBindings, dbinding);
    }
    ksai_print("Starting Des set layout binding 1b");
    for (auto& u : inResources.sampled_images) {
        uint32_t set = inGlslCompiler.get_decoration(u.id, spv::DecorationDescriptorSet);
        uint32_t binding = inGlslCompiler.get_decoration(u.id, spv::DecorationBinding);
        auto dbinding = vk::DescriptorSetLayoutBinding(binding, vk::DescriptorType::eCombinedImageSampler, 1, ShaderStageFlag, nullptr);
        CheckAndPushBindings(inDescriptorSetLayoutBindings, dbinding);
    }
    ksai_print("Starting Des set layout binding 1c");
    for (auto& u : inResources.storage_buffers) {
        uint32_t set = inGlslCompiler.get_decoration(u.id, spv::DecorationDescriptorSet);
        uint32_t binding = inGlslCompiler.get_decoration(u.id, spv::DecorationBinding);
        auto dbinding = vk::DescriptorSetLayoutBinding(binding, vk::DescriptorType::eStorageBuffer, 1, ShaderStageFlag);
        CheckAndPushBindings(inDescriptorSetLayoutBindings, dbinding);
    }
    ksai_print("Starting Des set layout binding 1d");
    for (auto& u : inResources.storage_images) {
        uint32_t set = inGlslCompiler.get_decoration(u.id, spv::DecorationDescriptorSet);
        uint32_t binding = inGlslCompiler.get_decoration(u.id, spv::DecorationBinding);
        auto dbinding = vk::DescriptorSetLayoutBinding(binding, vk::DescriptorType::eStorageImage, 1, ShaderStageFlag, nullptr);
        CheckAndPushBindings(inDescriptorSetLayoutBindings, dbinding);
    }
    ksai_print("Starting Des set layout binding 1e");
    for (auto& u : inResources.separate_samplers) {
        uint32_t set = inGlslCompiler.get_decoration(u.id, spv::DecorationDescriptorSet);
        uint32_t binding = inGlslCompiler.get_decoration(u.id, spv::DecorationBinding);
        auto dbinding = vk::DescriptorSetLayoutBinding(binding, vk::DescriptorType::eSampler, 1, ShaderStageFlag, nullptr);
        CheckAndPushBindings(inDescriptorSetLayoutBindings, dbinding);
    }
    ksai_print("Starting Des set layout binding 2");

    ksai_print("Finished Des set layout binding");
}
}
