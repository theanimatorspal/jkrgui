#pragma once
#include "Instance.hpp"
#include <Vulkan/Pipeline/VulkanPipelineCache.hpp>

namespace Jkr {
class ShaderModules {
       public:
               ShaderModules(const VulkanDevice& inDevice,
                             const std::vector<uint32_t>& inVertexShaderSPIRV,
                             const std::vector<uint32_t>& inFragmentShaderSPIRV,
                             const std::vector<uint32_t>& inComputeShaderSPIRV);
               ~ShaderModules() = default;

       public:
               GETTER& GetShaderModulesArray() const { return mVertexFragmentShaderModules; }

       public:
               GETTER& GetComputeShaderModuleArray() const { return mComputeShaderModule; }

       private:
               const VulkanDevice& mDevice;
               std::array<VulkanShaderModule, 2> mVertexFragmentShaderModules;
               std::array<VulkanShaderModule, 1> mComputeShaderModule;
};
} // namespace Jkr

namespace Jkr {
class PainterCache {
       public:
               explicit PainterCache(const Instance& inInstance, PipelinePropertiesContext inContext = PipelinePropertiesContext::Default);
               PainterCache& Store(const std::string& fileName, const std::string& inVertexShader, const std::string& inFragmentShader, const std::string& inComputeShader);
               PainterCache& Load(const std::string& fileName);

#ifdef JKR_USE_VARIABLE_DES_INDEXING
               PainterCache& __var_des_index_Store_EXT(const std::string& filename,
                                                       const std::string& invertexshader,
                                                       const std::string& infragmentshader,
                                                       const std::string& incomputeshader,
                                                       uint32_t inVarableDescriptorCount = 5000);
               PainterCache& __var_des_index_Load_EXT(const std::string& fileName, uint32_t inVarableDescriptorCount = 5000);
#endif
               GETTER& GetPipelineContext() const { return mPipelineContext; }
               GETTER& GetPipelineCache() const { return *mPtrPipelineCache; }

       public:
               GETTER& GetVertexFragmentDescriptorSetLayout() const { return *mPtrVertexFragmentDescriptorSetLayout; }
               GETTER& GetVertexFragmentPipelineLayout() const { return *mPtrVertexFragmentPipelineLayout; }
               GETTER& GetVertexFragmentShaderModulesArray() const { return mShaderModulesPtr->GetShaderModulesArray(); }

       public:
               GETTER& GetComputePipelineDescriptorSetLayout() const { return *mPtrComputeDescriptorSetLayout; }
               GETTER& GetComputePipelineLayout() const { return *mPtrComputePipelineLayout; }
               GETTER& GetComputePipelineModule() const { return mShaderModulesPtr->GetComputeShaderModuleArray(); }

       private:
               const Instance& mInstance;
               Up<ShaderModules> mShaderModulesPtr;
               VulkanPipelineContext mPipelineContext;
               Up<VulkanPipelineCache> mPtrPipelineCache = nullptr;

       private:
               using VertexFragmentDescriptorSetLayout = VulkanDescriptorSetLayout<2, ShaderModuleContext::Vertex, ShaderModuleContext::Fragment>;
               std::array<std::vector<uint32_t>, 2> mVertexFragmentShaderSPIRV = {};
               void StoreSPIRVsToFile(const std::string inFileName);
               void LoadSPIRVsFromFile(const std::string inFileName);
               Up<VertexFragmentDescriptorSetLayout> mPtrVertexFragmentDescriptorSetLayout = nullptr;
               Up<VertexFragmentDescriptorSetLayout> mPtrVertexFragmentDescriptorSetLayout_EXT = nullptr;
               Up<VulkanPipelineLayout<2>> mPtrVertexFragmentPipelineLayout = nullptr;

       private:
               using ComputeDescriptorSetLayout = VulkanDescriptorSetLayout<1, ShaderModuleContext::Compute>;
               std::array<std::vector<uint32_t>, 1> mComputeShaderSPIRV = {};
               Up<ComputeDescriptorSetLayout> mPtrComputeDescriptorSetLayout = nullptr;
               Up<ComputeDescriptorSetLayout> mPtrComputeDescriptorSetLayout_EXT = nullptr;
               Up<VulkanPipelineLayout<1>> mPtrComputePipelineLayout = nullptr;
};
} // namespace Jkr
