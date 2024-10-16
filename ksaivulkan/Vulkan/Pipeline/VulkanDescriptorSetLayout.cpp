#include "VulkanDescriptorSetLayout.hpp"

using namespace ksai;

constexpr auto NoOfShaderModules = 2;
constexpr int MaxSetCount        = 4;

void VulkanDescriptorSetLayoutBase::FillDescriptorLayoutBindings(
     ShaderModuleContext Context,
     const spirv_cross::ShaderResources &inResources,
     const spirv_cross::Compiler &inGlslCompiler,
     std::vector<vk::DescriptorSetLayoutBinding> &inDescriptorSetLayoutBindings,
     int inSet) {
    auto ShaderStageFlag = vk::ShaderStageFlagBits::eVertex;
    if (Context == ShaderModuleContext::Vertex)
        ShaderStageFlag = vk::ShaderStageFlagBits::eVertex;
    else if (Context == ShaderModuleContext::Fragment)
        ShaderStageFlag = vk::ShaderStageFlagBits::eFragment;
    else if (Context == ShaderModuleContext::Compute)
        ShaderStageFlag = vk::ShaderStageFlagBits::eCompute;

    for (auto &u : inResources.uniform_buffers) {
        uint32_t set     = inGlslCompiler.get_decoration(u.id, spv::DecorationDescriptorSet);
        uint32_t binding = inGlslCompiler.get_decoration(u.id, spv::DecorationBinding);
        auto dbinding    = vk::DescriptorSetLayoutBinding(
             binding, vk::DescriptorType::eUniformBuffer, 1, ShaderStageFlag, nullptr);
        if (set == inSet) CheckAndPushBindings(inDescriptorSetLayoutBindings, dbinding);
    }
    for (auto &u : inResources.sampled_images) {
        uint32_t set     = inGlslCompiler.get_decoration(u.id, spv::DecorationDescriptorSet);
        uint32_t binding = inGlslCompiler.get_decoration(u.id, spv::DecorationBinding);
        auto dbinding    = vk::DescriptorSetLayoutBinding(
             binding, vk::DescriptorType::eCombinedImageSampler, 1, ShaderStageFlag, nullptr);
        if (set == inSet) CheckAndPushBindings(inDescriptorSetLayoutBindings, dbinding);
    }
    for (auto &u : inResources.storage_buffers) {
        uint32_t set     = inGlslCompiler.get_decoration(u.id, spv::DecorationDescriptorSet);
        uint32_t binding = inGlslCompiler.get_decoration(u.id, spv::DecorationBinding);
        auto dbinding    = vk::DescriptorSetLayoutBinding(
             binding, vk::DescriptorType::eStorageBuffer, 1, ShaderStageFlag);
        if (set == inSet) CheckAndPushBindings(inDescriptorSetLayoutBindings, dbinding);
    }
    for (auto &u : inResources.storage_images) {
        uint32_t set     = inGlslCompiler.get_decoration(u.id, spv::DecorationDescriptorSet);
        uint32_t binding = inGlslCompiler.get_decoration(u.id, spv::DecorationBinding);
        auto dbinding    = vk::DescriptorSetLayoutBinding(
             binding, vk::DescriptorType::eStorageImage, 1, ShaderStageFlag, nullptr);
        if (set == inSet) CheckAndPushBindings(inDescriptorSetLayoutBindings, dbinding);
    }
    for (auto &u : inResources.separate_samplers) {
        uint32_t set     = inGlslCompiler.get_decoration(u.id, spv::DecorationDescriptorSet);
        uint32_t binding = inGlslCompiler.get_decoration(u.id, spv::DecorationBinding);
        auto dbinding    = vk::DescriptorSetLayoutBinding(
             binding, vk::DescriptorType::eSampler, 1, ShaderStageFlag, nullptr);
        if (set == inSet) CheckAndPushBindings(inDescriptorSetLayoutBindings, dbinding);
    }
}

template <>
VulkanDescriptorSetLayout<NoOfShaderModules,
                          ShaderModuleContext::Vertex,
                          ShaderModuleContext::Fragment>::
     VulkanDescriptorSetLayout(VulkanDevice &inDevice,
                               const std::vector<VulkanShaderModule> &inModules)
    : VulkanDescriptorSetLayoutBase(inDevice), mModules(inModules) {
    auto &VertexShaderCompiler    = mModules[0].GetShaderResourcesCompilerHandle();
    auto &VertexShaderResources   = mModules[0].GetShaderResourcesHandle();
    auto &FragmentShaderCompiler  = mModules[1].GetShaderResourcesCompilerHandle();
    auto &FragmentShaderResources = mModules[1].GetShaderResourcesHandle();

    for (int set = 0; set < MaxSetCount; ++set) {
        std::vector<vk::DescriptorSetLayoutBinding> DescriptorSetLayoutBindings;
        FillDescriptorLayoutBindings(ShaderModuleContext::Vertex,
                                     VertexShaderResources,
                                     VertexShaderCompiler,
                                     DescriptorSetLayoutBindings,
                                     set);
        FillDescriptorLayoutBindings(ShaderModuleContext::Fragment,
                                     FragmentShaderResources,
                                     FragmentShaderCompiler,
                                     DescriptorSetLayoutBindings,
                                     set);
        auto DescriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo(
             vk::DescriptorSetLayoutCreateFlags(), DescriptorSetLayoutBindings);
        mDescriptorSetLayouts.push_back(
             mDevice.createDescriptorSetLayout(DescriptorSetLayoutCreateInfo));
    }
}

template <>
VulkanDescriptorSetLayout<1, ShaderModuleContext::Compute>::VulkanDescriptorSetLayout(
     VulkanDevice &inDevice, const std::vector<VulkanShaderModule> &inModules)
    : VulkanDescriptorSetLayoutBase(inDevice), mModules(inModules) {

    for (int set = 0; set < MaxSetCount; ++set) {
        std::vector<vk::DescriptorSetLayoutBinding> DescriptorSetLayoutBindings;
        auto &ComputeShaderCompiler  = mModules[0].GetShaderResourcesCompilerHandle();
        auto &ComputeShaderResources = mModules[0].GetShaderResourcesHandle();
        FillDescriptorLayoutBindings(ShaderModuleContext::Compute,
                                     ComputeShaderResources,
                                     ComputeShaderCompiler,
                                     DescriptorSetLayoutBindings);
        auto DescriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo(
             vk::DescriptorSetLayoutCreateFlags(), DescriptorSetLayoutBindings);
        mDescriptorSetLayouts.push_back(
             mDevice.createDescriptorSetLayout(DescriptorSetLayoutCreateInfo));
    }
}

// TODO DO the above changes below as well

#ifdef USE_VARIABLE_DESCRIPTOR_INDEXING_FEATURE

template <>
VulkanDescriptorSetLayout<NoOfShaderModules,
                          ShaderModuleContext::Vertex,
                          ShaderModuleContext::Fragment>::
     VulkanDescriptorSetLayout(VulkanDevice &inDevice,
                               const std::vector<VulkanShaderModule> &inModules,
                               uint32_t inVariableSizedDescriptorMaxCount)
    : VulkanDescriptorSetLayoutBase(inDevice), mModules(inModules) {
    std::vector<vk::DescriptorSetLayoutBinding> DescriptorSetLayoutBindings;
    auto &VertexShaderCompiler    = mModules[0].GetShaderResourcesCompilerHandle();
    auto &VertexShaderResources   = mModules[0].GetShaderResourcesHandle();
    auto &FragmentShaderCompiler  = mModules[1].GetShaderResourcesCompilerHandle();
    auto &FragmentShaderResources = mModules[1].GetShaderResourcesHandle();
    FillDescriptorLayoutBindings(ShaderModuleContext::Vertex,
                                 VertexShaderResources,
                                 VertexShaderCompiler,
                                 DescriptorSetLayoutBindings);
    FillDescriptorLayoutBindings(ShaderModuleContext::Fragment,
                                 FragmentShaderResources,
                                 FragmentShaderCompiler,
                                 DescriptorSetLayoutBindings);

    /* For Variable Descriptor Sets */
    std::vector<vk::DescriptorBindingFlags> Flags;
    Flags.resize(DescriptorSetLayoutBindings.size(), static_cast<vk::DescriptorBindingFlags>(0));
    Flags[Flags.size() - 1] = vk::DescriptorBindingFlagBits::eVariableDescriptorCount |
                              vk::DescriptorBindingFlagBits::ePartiallyBound |
                              vk::DescriptorBindingFlagBits::eUpdateUnusedWhilePending;
    DescriptorSetLayoutBindings[Flags.size() - 1].setDescriptorCount(
         inVariableSizedDescriptorMaxCount);
    vk::DescriptorSetLayoutBindingFlagsCreateInfo BindingFlags(Flags);

    auto DescriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo(
         vk::DescriptorSetLayoutCreateFlags(), DescriptorSetLayoutBindings);

    vk::StructureChain<vk::DescriptorSetLayoutCreateInfo,
                       vk::DescriptorSetLayoutBindingFlagsCreateInfo>
         Chain(DescriptorSetLayoutCreateInfo, BindingFlags);

    mDescriptorSetLayouts.push_back(
         mDevice.createDescriptorSetLayout(Chain.get<vk::DescriptorSetLayoutCreateInfo>()));
}

template <>
VulkanDescriptorSetLayout<1, ShaderModuleContext::Compute>::VulkanDescriptorSetLayout(
     VulkanDevice &inDevice,
     const std::vector<VulkanShaderModule> &inModules,
     uint32_t inVariableSizedDescriptorMaxCount)
    : VulkanDescriptorSetLayoutBase(inDevice), mModules(inModules) {
    std::vector<vk::DescriptorSetLayoutBinding> DescriptorSetLayoutBindings;
    auto &ComputeShaderCompiler  = mModules[0].GetShaderResourcesCompilerHandle();
    auto &ComputeShaderResources = mModules[0].GetShaderResourcesHandle();
    FillDescriptorLayoutBindings(ShaderModuleContext::Compute,
                                 ComputeShaderResources,
                                 ComputeShaderCompiler,
                                 DescriptorSetLayoutBindings);

    std::vector<vk::DescriptorBindingFlags> Flags;
    Flags.resize(DescriptorSetLayoutBindings.size(), static_cast<vk::DescriptorBindingFlags>(0));
    if (not Flags.empty()) {
        Flags[Flags.size() - 1] = vk::DescriptorBindingFlagBits::eVariableDescriptorCount |
                                  vk::DescriptorBindingFlagBits::ePartiallyBound |
                                  vk::DescriptorBindingFlagBits::eUpdateUnusedWhilePending;
        DescriptorSetLayoutBindings[Flags.size() - 1].setDescriptorCount(
             inVariableSizedDescriptorMaxCount);
    }
    vk::DescriptorSetLayoutBindingFlagsCreateInfo BindingFlags(Flags);
    auto DescriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo(
         vk::DescriptorSetLayoutCreateFlags(), DescriptorSetLayoutBindings);

    vk::StructureChain<vk::DescriptorSetLayoutCreateInfo,
                       vk::DescriptorSetLayoutBindingFlagsCreateInfo>
         Chain(DescriptorSetLayoutCreateInfo, BindingFlags);
    mDescriptorSetLayouts.push_back(
         mDevice.createDescriptorSetLayout(Chain.get<vk::DescriptorSetLayoutCreateInfo>()));
}
#endif

ksai::VulkanDescriptorSetLayoutBase::VulkanDescriptorSetLayoutBase(VulkanDevice &inDevice)
    : mDevice(inDevice.GetDeviceHandle()) {}

ksai::VulkanDescriptorSetLayoutBase::~VulkanDescriptorSetLayoutBase() {
    for (auto &DescLayout : mDescriptorSetLayouts) {
        mDevice.destroyDescriptorSetLayout(DescLayout);
    }
}

void ksai::VulkanDescriptorSetLayoutBase::FillDescriptorLayoutBindings(
     const spirv_cross::ShaderResources &inResources,
     const spirv_cross::Compiler &inGlslComp,
     std::vector<vk::DescriptorSetLayoutBinding> &DescriptorSetLayoutBindings,
     int inSet) {
    FillDescriptorLayoutBindings(
         ShaderModuleContext::Vertex, inResources, inGlslComp, DescriptorSetLayoutBindings, inSet);
}

void ksai::VulkanDescriptorSetLayoutBase::CheckAndPushBindings(
     std::vector<vk::DescriptorSetLayoutBinding> &inDescriptorSetLayoutBindings,
     vk::DescriptorSetLayoutBinding inBinding) {
    auto SearchIndex           = std::find_if(inDescriptorSetLayoutBindings.begin(),
                                    inDescriptorSetLayoutBindings.end(),
                                    [&](vk::DescriptorSetLayoutBinding &Binding) {
                                        return Binding.binding == inBinding.binding;
                                    });

    bool BindingAlreadyPresent = (SearchIndex != inDescriptorSetLayoutBindings.end());

    if (BindingAlreadyPresent) {
        SearchIndex->stageFlags = SearchIndex->stageFlags | inBinding.stageFlags;
    } else {
        inDescriptorSetLayoutBindings.push_back(inBinding);
    }
}
