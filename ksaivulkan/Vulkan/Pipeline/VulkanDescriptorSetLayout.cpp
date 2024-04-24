#include "VulkanDescriptorSetLayout.hpp"

using namespace ksai;

constexpr auto NoOfShaderModules = 2;

template <>
VulkanDescriptorSetLayout<NoOfShaderModules, ShaderModuleContext::Vertex, ShaderModuleContext::Fragment>::VulkanDescriptorSetLayout(const VulkanDevice& inDevice,
                                                                                                                                    const std::vector<VulkanShaderModule>& inModules)
    : VulkanDescriptorSetLayoutBase(inDevice), mModules(inModules) {
               std::vector<vk::DescriptorSetLayoutBinding> DescriptorSetLayoutBindings;
               auto& VertexShaderCompiler    = mModules[0].GetShaderResourcesCompilerHandle();
               auto& VertexShaderResources   = mModules[0].GetShaderResourcesHandle();
               auto& FragmentShaderCompiler  = mModules[1].GetShaderResourcesCompilerHandle();
               auto& FragmentShaderResources = mModules[1].GetShaderResourcesHandle();
               FillDescriptorLayoutBindings<ShaderModuleContext::Vertex>(VertexShaderResources, VertexShaderCompiler, DescriptorSetLayoutBindings);
               FillDescriptorLayoutBindings<ShaderModuleContext::Fragment>(FragmentShaderResources, FragmentShaderCompiler, DescriptorSetLayoutBindings);

               auto DescriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo(vk::DescriptorSetLayoutCreateFlags(), DescriptorSetLayoutBindings);

               mDescriptorSetLayout               = mDevice.createDescriptorSetLayout(DescriptorSetLayoutCreateInfo);
}

template <>
VulkanDescriptorSetLayout<1, ShaderModuleContext::Compute>::VulkanDescriptorSetLayout(const VulkanDevice& inDevice, const std::vector<VulkanShaderModule>& inModules)
    : VulkanDescriptorSetLayoutBase(inDevice), mModules(inModules) {
               std::vector<vk::DescriptorSetLayoutBinding> DescriptorSetLayoutBindings;
               auto& ComputeShaderCompiler  = mModules[0].GetShaderResourcesCompilerHandle();
               auto& ComputeShaderResources = mModules[0].GetShaderResourcesHandle();
               FillDescriptorLayoutBindings<ShaderModuleContext::Compute>(ComputeShaderResources, ComputeShaderCompiler, DescriptorSetLayoutBindings);
               auto DescriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo(vk::DescriptorSetLayoutCreateFlags(), DescriptorSetLayoutBindings);
               mDescriptorSetLayout               = mDevice.createDescriptorSetLayout(DescriptorSetLayoutCreateInfo);
}

#ifdef USE_VARIABLE_DESCRIPTOR_INDEXING_FEATURE

template <>
VulkanDescriptorSetLayout<NoOfShaderModules, ShaderModuleContext::Vertex, ShaderModuleContext::Fragment>::VulkanDescriptorSetLayout(const VulkanDevice& inDevice,
                                                                                                                                    const std::vector<VulkanShaderModule>& inModules,
                                                                                                                                    uint32_t inVariableSizedDescriptorMaxCount)
    : VulkanDescriptorSetLayoutBase(inDevice), mModules(inModules) {
               std::vector<vk::DescriptorSetLayoutBinding> DescriptorSetLayoutBindings;
               auto& VertexShaderCompiler    = mModules[0].GetShaderResourcesCompilerHandle();
               auto& VertexShaderResources   = mModules[0].GetShaderResourcesHandle();
               auto& FragmentShaderCompiler  = mModules[1].GetShaderResourcesCompilerHandle();
               auto& FragmentShaderResources = mModules[1].GetShaderResourcesHandle();
               FillDescriptorLayoutBindings<ShaderModuleContext::Vertex>(VertexShaderResources, VertexShaderCompiler, DescriptorSetLayoutBindings);
               FillDescriptorLayoutBindings<ShaderModuleContext::Fragment>(FragmentShaderResources, FragmentShaderCompiler, DescriptorSetLayoutBindings);

               /* For Variable Descriptor Sets */
               std::vector<vk::DescriptorBindingFlags> Flags;
               Flags.resize(DescriptorSetLayoutBindings.size(), static_cast<vk::DescriptorBindingFlags>(0));
               Flags[Flags.size() - 1] =
                         vk::DescriptorBindingFlagBits::eVariableDescriptorCount | vk::DescriptorBindingFlagBits::ePartiallyBound | vk::DescriptorBindingFlagBits::eUpdateUnusedWhilePending;
               DescriptorSetLayoutBindings[Flags.size() - 1].setDescriptorCount(inVariableSizedDescriptorMaxCount);
               vk::DescriptorSetLayoutBindingFlagsCreateInfo BindingFlags(Flags);

               auto DescriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo(vk::DescriptorSetLayoutCreateFlags(), DescriptorSetLayoutBindings);

               vk::StructureChain<vk::DescriptorSetLayoutCreateInfo, vk::DescriptorSetLayoutBindingFlagsCreateInfo> Chain(DescriptorSetLayoutCreateInfo, BindingFlags);

               mDescriptorSetLayout = mDevice.createDescriptorSetLayout(Chain.get<vk::DescriptorSetLayoutCreateInfo>());
}

template <>
VulkanDescriptorSetLayout<1, ShaderModuleContext::Compute>::VulkanDescriptorSetLayout(const VulkanDevice& inDevice,
                                                                                      const std::vector<VulkanShaderModule>& inModules,
                                                                                      uint32_t inVariableSizedDescriptorMaxCount)
    : VulkanDescriptorSetLayoutBase(inDevice), mModules(inModules) {
               std::vector<vk::DescriptorSetLayoutBinding> DescriptorSetLayoutBindings;
               auto& ComputeShaderCompiler  = mModules[0].GetShaderResourcesCompilerHandle();
               auto& ComputeShaderResources = mModules[0].GetShaderResourcesHandle();
               FillDescriptorLayoutBindings<ShaderModuleContext::Compute>(ComputeShaderResources, ComputeShaderCompiler, DescriptorSetLayoutBindings);

               std::vector<vk::DescriptorBindingFlags> Flags;
               Flags.resize(DescriptorSetLayoutBindings.size(), static_cast<vk::DescriptorBindingFlags>(0));
               if (not Flags.empty()) {
                              Flags[Flags.size() - 1] = vk::DescriptorBindingFlagBits::eVariableDescriptorCount | vk::DescriptorBindingFlagBits::ePartiallyBound |
                                                        vk::DescriptorBindingFlagBits::eUpdateUnusedWhilePending;
                              DescriptorSetLayoutBindings[Flags.size() - 1].setDescriptorCount(inVariableSizedDescriptorMaxCount);
               }
               vk::DescriptorSetLayoutBindingFlagsCreateInfo BindingFlags(Flags);
               auto DescriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo(vk::DescriptorSetLayoutCreateFlags(), DescriptorSetLayoutBindings);

               vk::StructureChain<vk::DescriptorSetLayoutCreateInfo, vk::DescriptorSetLayoutBindingFlagsCreateInfo> Chain(DescriptorSetLayoutCreateInfo, BindingFlags);
               mDescriptorSetLayout = mDevice.createDescriptorSetLayout(Chain.get<vk::DescriptorSetLayoutCreateInfo>());
}
#endif

ksai::VulkanDescriptorSetLayoutBase::VulkanDescriptorSetLayoutBase(const VulkanDevice& inDevice) : mDevice(inDevice.GetDeviceHandle()) {}

ksai::VulkanDescriptorSetLayoutBase::~VulkanDescriptorSetLayoutBase() { mDevice.destroyDescriptorSetLayout(mDescriptorSetLayout); }

void ksai::VulkanDescriptorSetLayoutBase::FillDescriptorLayoutBindings(const spirv_cross::ShaderResources& inResources,
                                                                       const spirv_cross::Compiler& inGlslComp,
                                                                       std::vector<vk::DescriptorSetLayoutBinding>& DescriptorSetLayoutBindings) {
               FillDescriptorLayoutBindings<ShaderModuleContext::Vertex>(inResources, inGlslComp, DescriptorSetLayoutBindings);
}

void ksai::VulkanDescriptorSetLayoutBase::CheckAndPushBindings(std::vector<vk::DescriptorSetLayoutBinding>& inDescriptorSetLayoutBindings, vk::DescriptorSetLayoutBinding inBinding) {
               auto SearchIndex = std::find_if(
                         inDescriptorSetLayoutBindings.begin(), inDescriptorSetLayoutBindings.end(), [&](vk::DescriptorSetLayoutBinding& Binding) { return Binding.binding == inBinding.binding; });

               bool BindingAlreadyPresent = (SearchIndex != inDescriptorSetLayoutBindings.end());

               if (BindingAlreadyPresent) {
                              SearchIndex->stageFlags = SearchIndex->stageFlags | inBinding.stageFlags;
               } else {
                              inDescriptorSetLayoutBindings.push_back(inBinding);
               }
}
