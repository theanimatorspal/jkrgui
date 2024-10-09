#pragma once
#include "Instance.hpp"
#include <Vulkan/Pipeline/VulkanDescriptorSetLayout.hpp>
#include <Vulkan/Pipeline/VulkanPipeline.hpp>
#include <Vulkan/Pipeline/VulkanPipelineCache.hpp>
#include <Vulkan/Pipeline/VulkanPipelineContext.hpp>
#include <Vulkan/Pipeline/VulkanShaderModule.hpp>

namespace Jkr {
class ShaderCompiler {
    public:
    ShaderCompiler(const std::string_view inVertexShaderString,
                   const std::string_view inFragmentShaderString,
                   std::vector<uint32_t> &outVertexShaderModule,
                   std::vector<uint32_t> &outFragmentShaderModule);
    ShaderCompiler(const std::string_view inComputeShaderString,
                   std::vector<uint32_t> &outComputeShaderModule);
};

class ShaderModules {
    public:
    ShaderModules(const VulkanDevice &inDevice,
                  const std::vector<uint32_t> &inVertexShaderSPIRV   = {},
                  const std::vector<uint32_t> &inFragmentShaderSPIRV = {},
                  const std::vector<uint32_t> &inComputeShaderSPIRV  = {});
    ~ShaderModules();

    public:
    GETTER &GetShaderModulesArray() const { return mVertexFragmentShaderModules; }

    public:
    GETTER &GetComputeShaderModuleArray() const { return mComputeShaderModule; }

    private:
    const VulkanDevice &mDevice;
    std::vector<VulkanShaderModule> mVertexFragmentShaderModules;
    std::vector<VulkanShaderModule> mComputeShaderModule;
};

namespace Misc {
class FileJkr;
}

class PainterCache {
    public:
    explicit PainterCache(Instance &inInstance,
                          PipelinePropertiesContext inContext = PipelinePropertiesContext::Default);
    void SetCacheFile(Jkr::Misc::FileJkr *inFileJkr) { mCacheFile = inFileJkr; };
    void WriteToCacheFile(Jkr::Misc::FileJkr &inFileJkr, std::string inEntry);
    void ReadFromCacheFile(Jkr::Misc::FileJkr &inFileJkr, std::string inEntry);
    std::string GetCacheFileEntryName() const { return mCacheFileEntryName; }
    GETTER GetCacheFile() { return mCacheFile; }
    PainterCache &Store(const std::string_view fileName,
                        const std::string_view inVertexShader   = "",
                        const std::string_view inFragmentShader = "",
                        const std::string_view inComputeShader  = "",
                        ui inVarDescount                        = -1);
    PainterCache &Load(const std::string &fileName, ui inVarDescount = -1);

    GETTER &GetPipelineContext() const { return mPipelineContext; }
    GETTER &GetPipelineCache() const { return *mPtrPipelineCache; }
    GETTER &GetVertexFragmentDescriptorSetLayout() const {
        return *mPtrVertexFragmentDescriptorSetLayout;
    }
    GETTER &GetVertexFragmentPipelineLayout() const { return *mPtrVertexFragmentPipelineLayout; }
    GETTER &GetVertexFragmentShaderModulesArray() const {
        return mShaderModulesPtr->GetShaderModulesArray();
    }
    GETTER &GetComputePipelineDescriptorSetLayout() const {
        return *mPtrComputeDescriptorSetLayout;
    }
    GETTER &GetComputePipelineLayout() const { return *mPtrComputePipelineLayout; }
    GETTER &GetComputePipelineModule() const {
        return mShaderModulesPtr->GetComputeShaderModuleArray();
    }

    private:
    Instance &mInstance;
    Up<ShaderModules> mShaderModulesPtr;
    VulkanPipelineContext mPipelineContext;
    Up<VulkanPipelineCache> mPtrPipelineCache = nullptr;

    private:
    using VertexFragmentDescriptorSetLayout =
         VulkanDescriptorSetLayout<2, ShaderModuleContext::Vertex, ShaderModuleContext::Fragment>;
    std::array<std::vector<uint32_t>, 2> mVertexFragmentShaderSPIRV = {};
    void StoreSPIRVsToFile(const std::string_view inFileName);
    void LoadSPIRVsFromFile(const std::string_view inFileName);
    Up<VertexFragmentDescriptorSetLayout> mPtrVertexFragmentDescriptorSetLayout     = nullptr;
    Up<VertexFragmentDescriptorSetLayout> mPtrVertexFragmentDescriptorSetLayout_EXT = nullptr;
    Up<VulkanPipelineLayout<2>> mPtrVertexFragmentPipelineLayout                    = nullptr;

    private:
    using ComputeDescriptorSetLayout = VulkanDescriptorSetLayout<1, ShaderModuleContext::Compute>;
    std::array<std::vector<uint32_t>, 1> mComputeShaderSPIRV          = {};
    Up<ComputeDescriptorSetLayout> mPtrComputeDescriptorSetLayout     = nullptr;
    Up<ComputeDescriptorSetLayout> mPtrComputeDescriptorSetLayout_EXT = nullptr;
    Up<VulkanPipelineLayout<1>> mPtrComputePipelineLayout             = nullptr;

    private:
    Jkr::Misc::FileJkr *mCacheFile = nullptr;
    std::string mCacheFileEntryName;
};
} // namespace Jkr
