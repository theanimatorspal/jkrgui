#include "VulkanPipelineLayout.hpp"

using namespace ksai;

template <>
VulkanPipelineLayout<2>::VulkanPipelineLayout(VulkanDevice &inDevice,
                                              VulkanDescriptorSetLayoutBase &inDescriptorSetLayout,
                                              const std::vector<VulkanShaderModule> &inModules)
    : VulkanPipelineLayoutBase(inDevice), mModules(inModules) {
    std::vector<vk::PushConstantRange> PushConstantRanges;
    auto &VertexCompiler    = mModules[0].GetShaderResourcesCompilerHandle();
    auto &VertexResources   = mModules[0].GetShaderResourcesHandle();
    auto &FragmentCompiler  = mModules[1].GetShaderResourcesCompilerHandle();
    auto &FragmentResources = mModules[1].GetShaderResourcesHandle();
    FillPushConstantRanges(
         VertexResources, VertexCompiler, FragmentResources, FragmentCompiler, PushConstantRanges);
    auto PipelineLayoutCreateInfo =
         vk::PipelineLayoutCreateInfo()
              .setSetLayouts(inDescriptorSetLayout.GetDescriptorLayoutHandle())
              .setPushConstantRanges(PushConstantRanges);
    mPipelineLayout = mDevice->createPipelineLayout(PipelineLayoutCreateInfo);
}

template <>
VulkanPipelineLayout<1>::VulkanPipelineLayout(VulkanDevice &inDevice,
                                              VulkanDescriptorSetLayoutBase &inDescriptorSetLayout,
                                              const std::vector<VulkanShaderModule> &inModules)
    : VulkanPipelineLayoutBase(inDevice), mModules(inModules) {
    std::vector<vk::PushConstantRange> PushConstantRanges;
    auto &ComputeCompiler  = mModules[0].GetShaderResourcesCompilerHandle();
    auto &ComputeResources = mModules[0].GetShaderResourcesHandle();
    FillPushConstantRanges(
         ComputeResources, ComputeCompiler, ComputeResources, ComputeCompiler, PushConstantRanges);
    auto PipelineLayoutCreateInfo =
         vk::PipelineLayoutCreateInfo()
              .setSetLayouts(inDescriptorSetLayout.GetDescriptorLayoutHandle())
              .setPushConstantRanges(PushConstantRanges);
    mPipelineLayout = mDevice->createPipelineLayout(PipelineLayoutCreateInfo);
}

void ksai::VulkanPipelineLayoutBase::FillPushConstantRanges(
     const spirv_cross::ShaderResources &VertexResources,
     const spirv_cross::Compiler &inVertexComp,
     const spirv_cross::ShaderResources &inFragmentResources,
     const spirv_cross::Compiler &inFragmentComp,
     std::vector<vk::PushConstantRange> &PushConstantRanges) {
    auto ShaderStage = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment |
                       vk::ShaderStageFlagBits::eCompute;

    size_t size = VertexResources.push_constant_buffers.size();
    PushConstantRanges.reserve(size);
    for (int i = 0; i < size; i++) {
        auto &u = VertexResources.push_constant_buffers[i];
        PushConstantRanges.push_back(
             vk::PushConstantRange(ShaderStage)
                  .setOffset(inVertexComp.get_decoration(u.id, spv::DecorationOffset))
                  .setSize(
                       inVertexComp.get_declared_struct_size(inVertexComp.get_type(u.type_id))));
    }
}

void VulkanPipelineLayoutBase::Init(CreateInfo info) {
    mDevice      = &info.inDevice->GetDeviceHandle();
    mInitialized = true;
}

void VulkanPipelineLayoutBase::Destroy() {
    mDevice->destroyPipelineLayout(mPipelineLayout);
    mInitialized = false;
}

VulkanPipelineLayoutBase::~VulkanPipelineLayoutBase() {
    if (mInitialized) {
        Destroy();
    }
}