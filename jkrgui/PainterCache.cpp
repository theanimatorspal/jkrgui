#include "PainterCache.hpp"
#include <ksai_filestream.hpp>

Jkr::ShaderModules::ShaderModules(const VulkanDevice& inDevice,
                                  const std::vector<uint32_t>& inVertexShaderSPIRV,
                                  const std::vector<uint32_t>& inFragmentShaderSPIRV,
                                  const std::vector<uint32_t>& inComputeShaderSPIRV)
    : mDevice(inDevice) {
    mVertexFragmentShaderModules.push_back(
         VulkanShaderModule(inDevice, inVertexShaderSPIRV, false));
    mVertexFragmentShaderModules.push_back(
         VulkanShaderModule(inDevice, inFragmentShaderSPIRV, false));
    mComputeShaderModule.push_back(VulkanShaderModule(inDevice, inComputeShaderSPIRV, false));
}

Jkr::ShaderModules::~ShaderModules() {
    for (auto& m : mVertexFragmentShaderModules) {
        mDevice.GetDeviceHandle().destroyShaderModule(m.GetShaderModuleHandle());
    }
    for (auto& m : mComputeShaderModule) {
        mDevice.GetDeviceHandle().destroyShaderModule(m.GetShaderModuleHandle());
    }
}

Jkr::PainterCache::PainterCache(const Instance& inInstance, PipelinePropertiesContext inContext)
    : mInstance(inInstance), mPipelineContext(inContext) {}

#ifndef JKR_USE_VARIABLE_DES_INDEXING
Jkr::PainterCache& Jkr::PainterCache::Load(const std::string& fileName, ui inVardescount) {
    time.reset();
    LoadSPIRVsFromFile(fileName);

    std::vector<uint32_t> vf = mVertexFragmentShaderSPIRV[0];
    std::vector<uint32_t> ff = mVertexFragmentShaderSPIRV[1];
    std::vector<uint32_t> cf = mComputeShaderSPIRV[0];

    mShaderModulesPtr        = MakeUp<ShaderModules>(mInstance.GetDevice(),
                                              mVertexFragmentShaderSPIRV[0],
                                              mVertexFragmentShaderSPIRV[1],
                                              mComputeShaderSPIRV[0]);

    time.reset();
    mPtrVertexFragmentDescriptorSetLayout = MakeUp<
         VulkanDescriptorSetLayout<2, ShaderModuleContext::Vertex, ShaderModuleContext::Fragment>>(
         mInstance.GetDevice(), mShaderModulesPtr->GetShaderModulesArray());
    mPtrVertexFragmentPipelineLayout =
         MakeUp<VulkanPipelineLayout<2>>(mInstance.GetDevice(),
                                         *mPtrVertexFragmentDescriptorSetLayout,
                                         mShaderModulesPtr->GetShaderModulesArray());
    mPtrPipelineCache = MakeUp<VulkanPipelineCache>(mInstance.GetDevice(), fileName);

    mPtrComputeDescriptorSetLayout =
         MakeUp<VulkanDescriptorSetLayout<1, ShaderModuleContext::Compute>>(
              mInstance.GetDevice(), mShaderModulesPtr->GetComputeShaderModuleArray());
    mPtrComputePipelineLayout =
         MakeUp<VulkanPipelineLayout<1>>(mInstance.GetDevice(),
                                         *mPtrComputeDescriptorSetLayout,
                                         mShaderModulesPtr->GetComputeShaderModuleArray());
    time.elapsed("Shader Descriptor + Pipeline Reflection");

    return *this;
}

Jkr::PainterCache& Jkr::PainterCache::Store(const std::string& fileName,
                                            const std::string& inVertexShader,
                                            const std::string& inFragmentShader,
                                            const std::string& inComputeShader,
                                            ui inVarDescount) {
    time.reset();
    ShaderCompiler Compiler(inVertexShader,
                            inFragmentShader,
                            mVertexFragmentShaderSPIRV[0],
                            mVertexFragmentShaderSPIRV[1]);
    ShaderCompiler ComputeCompiler(inComputeShader, mComputeShaderSPIRV[0]);
    mShaderModulesPtr = MakeUp<ShaderModules>(mInstance.GetDevice(),
                                              mVertexFragmentShaderSPIRV[0],
                                              mVertexFragmentShaderSPIRV[1],
                                              mComputeShaderSPIRV[0]);
    time.elapsed("Shader Compilation");

    time.reset();
    mPtrVertexFragmentDescriptorSetLayout = MakeUp<
         VulkanDescriptorSetLayout<2, ShaderModuleContext::Vertex, ShaderModuleContext::Fragment>>(
         mInstance.GetDevice(), mShaderModulesPtr->GetShaderModulesArray());
    mPtrVertexFragmentPipelineLayout =
         MakeUp<VulkanPipelineLayout<2>>(mInstance.GetDevice(),
                                         *mPtrVertexFragmentDescriptorSetLayout,
                                         mShaderModulesPtr->GetShaderModulesArray());
    mPtrPipelineCache = MakeUp<VulkanPipelineCache>(mInstance.GetDevice(), fileName);

    mPtrComputeDescriptorSetLayout =
         MakeUp<VulkanDescriptorSetLayout<1, ShaderModuleContext::Compute>>(
              mInstance.GetDevice(), mShaderModulesPtr->GetComputeShaderModuleArray());
    mPtrComputePipelineLayout =
         MakeUp<VulkanPipelineLayout<1>>(mInstance.GetDevice(),
                                         *mPtrComputeDescriptorSetLayout,
                                         mShaderModulesPtr->GetComputeShaderModuleArray());
    time.elapsed("Shader Descriptor + Pipeline Reflection");

    StoreSPIRVsToFile(fileName);

    return *this;
}

#else
Jkr::PainterCache& Jkr::PainterCache::Load(const std::string& fileName,
                                           uint32_t inVariableDescirptorCount) {
    time.reset();
    LoadSPIRVsFromFile(fileName);

    mShaderModulesPtr = MakeUp<ShaderModules>(mInstance.GetDevice(),
                                              mVertexFragmentShaderSPIRV[0],
                                              mVertexFragmentShaderSPIRV[1],
                                              mComputeShaderSPIRV[0]);
    time.elapsed("Shader Compilation Load Cached");

    time.reset();
    mPtrVertexFragmentDescriptorSetLayout = MakeUp<
         VulkanDescriptorSetLayout<2, ShaderModuleContext::Vertex, ShaderModuleContext::Fragment>>(
         mInstance.GetDevice(),
         mShaderModulesPtr->GetShaderModulesArray(),
         inVariableDescirptorCount);
    mPtrVertexFragmentPipelineLayout =
         MakeUp<VulkanPipelineLayout<2>>(mInstance.GetDevice(),
                                         *mPtrVertexFragmentDescriptorSetLayout,
                                         mShaderModulesPtr->GetShaderModulesArray());
    mPtrPipelineCache = MakeUp<VulkanPipelineCache>(mInstance.GetDevice(), fileName);

    mPtrComputeDescriptorSetLayout =
         MakeUp<VulkanDescriptorSetLayout<1, ShaderModuleContext::Compute>>(
              mInstance.GetDevice(),
              mShaderModulesPtr->GetComputeShaderModuleArray(),
              inVariableDescirptorCount);
    mPtrComputePipelineLayout =
         MakeUp<VulkanPipelineLayout<1>>(mInstance.GetDevice(),
                                         *mPtrComputeDescriptorSetLayout,
                                         mShaderModulesPtr->GetComputeShaderModuleArray());
    time.elapsed("Shader Descriptor + Pipeline Reflection");

    return *this;
}

Jkr::PainterCache& Jkr::PainterCache::Store(const std::string& fileName,
                                            const std::string& inVertexShader,
                                            const std::string& inFragmentShader,
                                            const std::string& inComputeShader,
                                            uint32_t inVariableDescriptorCount) {
    time.reset();
    ShaderCompiler Compiler(inVertexShader,
                            inFragmentShader,
                            mVertexFragmentShaderSPIRV[0],
                            mVertexFragmentShaderSPIRV[1]);
    ShaderCompiler ComputeCompiler(inComputeShader, mComputeShaderSPIRV[0]);
    mShaderModulesPtr = MakeUp<ShaderModules>(mInstance.GetDevice(),
                                              mVertexFragmentShaderSPIRV[0],
                                              mVertexFragmentShaderSPIRV[1],
                                              mComputeShaderSPIRV[0]);
    time.elapsed("Shader Compilation");

    time.reset();
    mPtrVertexFragmentDescriptorSetLayout = MakeUp<
         VulkanDescriptorSetLayout<2, ShaderModuleContext::Vertex, ShaderModuleContext::Fragment>>(
         mInstance.GetDevice(),
         mShaderModulesPtr->GetShaderModulesArray(),
         inVariableDescriptorCount);
    mPtrVertexFragmentPipelineLayout =
         MakeUp<VulkanPipelineLayout<2>>(mInstance.GetDevice(),
                                         *mPtrVertexFragmentDescriptorSetLayout,
                                         mShaderModulesPtr->GetShaderModulesArray());
    mPtrPipelineCache = MakeUp<VulkanPipelineCache>(mInstance.GetDevice(), fileName);

    mPtrComputeDescriptorSetLayout =
         MakeUp<VulkanDescriptorSetLayout<1, ShaderModuleContext::Compute>>(
              mInstance.GetDevice(),
              mShaderModulesPtr->GetComputeShaderModuleArray(),
              inVariableDescriptorCount);
    mPtrComputePipelineLayout =
         MakeUp<VulkanPipelineLayout<1>>(mInstance.GetDevice(),
                                         *mPtrComputeDescriptorSetLayout,
                                         mShaderModulesPtr->GetComputeShaderModuleArray());
    time.elapsed("Shader Descriptor + Pipeline Reflection");

    StoreSPIRVsToFile(fileName);

    return *this;
}

#endif

void Jkr::PainterCache::StoreSPIRVsToFile(const std::string inFileName) {
    ksai::fileoutputstream Fstream(inFileName, std::ios_base::binary);
    uint32_t size_V = mVertexFragmentShaderSPIRV[0].size() * sizeof(uint32_t);
    uint32_t size_F = mVertexFragmentShaderSPIRV[1].size() * sizeof(uint32_t);
    uint32_t size_C = mComputeShaderSPIRV[0].size() * sizeof(uint32_t);
    Fstream.write(reinterpret_cast<char*>(&size_V), sizeof(uint32_t));
    Fstream.write(reinterpret_cast<char*>(&size_F), sizeof(uint32_t));
    Fstream.write(reinterpret_cast<char*>(&size_C), sizeof(uint32_t));
    Fstream.write(reinterpret_cast<char*>(mVertexFragmentShaderSPIRV[0].data()),
                  mVertexFragmentShaderSPIRV[0].size() * sizeof(uint32_t));
    Fstream.write(reinterpret_cast<char*>(mVertexFragmentShaderSPIRV[1].data()),
                  mVertexFragmentShaderSPIRV[1].size() * sizeof(uint32_t));
    Fstream.write(reinterpret_cast<char*>(mComputeShaderSPIRV[0].data()),
                  mComputeShaderSPIRV[0].size() * sizeof(uint32_t));
}

void Jkr::PainterCache::LoadSPIRVsFromFile(const std::string inFileName) {
    ksai::fileinputstream Fstream(inFileName, std::ios_base::binary);
    uint32_t size_V = 0;
    uint32_t size_F = 0;
    uint32_t size_C = 0;
    Fstream.read(reinterpret_cast<char*>(&size_V), sizeof(uint32_t));
    Fstream.read(reinterpret_cast<char*>(&size_F), sizeof(uint32_t));
    Fstream.read(reinterpret_cast<char*>(&size_C), sizeof(uint32_t));

    mVertexFragmentShaderSPIRV[0].resize(size_V / sizeof(uint32_t));
    mVertexFragmentShaderSPIRV[1].resize(size_F / sizeof(uint32_t));
    mComputeShaderSPIRV[0].resize(size_C / sizeof(uint32_t));
    Fstream.read(reinterpret_cast<char*>(mVertexFragmentShaderSPIRV[0].data()),
                 mVertexFragmentShaderSPIRV[0].size() * sizeof(uint32_t));
    Fstream.read(reinterpret_cast<char*>(mVertexFragmentShaderSPIRV[1].data()),
                 mVertexFragmentShaderSPIRV[1].size() * sizeof(uint32_t));
    Fstream.read(reinterpret_cast<char*>(mComputeShaderSPIRV[0].data()),
                 mComputeShaderSPIRV[0].size() * sizeof(uint32_t));
}
