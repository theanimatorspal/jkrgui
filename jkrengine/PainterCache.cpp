#include "PainterCache.hpp"
#include <Misc/JkrFile.hpp>

using namespace Jkr;

ShaderCompiler::ShaderCompiler(const std::string_view inVertexShaderString,
                               const std::string_view inFragmentShaderString,
                               std::vector<uint32_t> &outVertexShaderModule,
                               std::vector<uint32_t> &outFragmentShaderModule) {
    if (inVertexShaderString != "" and inFragmentShaderString != "") {
        bool success = SpirvHelper::GLSLtoSPV(
             vk::ShaderStageFlagBits::eVertex, inVertexShaderString.data(), outVertexShaderModule);
        if (!success) Log("VertexShader Failed", "ERROR");
        success = SpirvHelper::GLSLtoSPV(vk::ShaderStageFlagBits::eFragment,
                                         inFragmentShaderString.data(),
                                         outFragmentShaderModule);
        if (!success) Log("FragmentShader Failed", "ERROR");
    }
}

ShaderCompiler::ShaderCompiler(const std::string_view inComputeShaderString,
                               std::vector<uint32_t> &outComputeShaderModule) {
    if (inComputeShaderString != "") {
        bool success = SpirvHelper::GLSLtoSPV(vk::ShaderStageFlagBits::eCompute,
                                              inComputeShaderString.data(),
                                              outComputeShaderModule);
        if (!success) Log("ComputeShader Failed", "ERROR");
    }
}

Jkr::ShaderModules::ShaderModules(VulkanDevice &inDevice,
                                  const std::vector<uint32_t> &inVertexShaderSPIRV,
                                  const std::vector<uint32_t> &inFragmentShaderSPIRV,
                                  const std::vector<uint32_t> &inComputeShaderSPIRV)
    : mDevice(inDevice) {
    if (inVertexShaderSPIRV.size() > 0 and inFragmentShaderSPIRV.size() > 0) {
        mVertexFragmentShaderModules.push_back(
             VulkanShaderModule(inDevice, inVertexShaderSPIRV, false));
        mVertexFragmentShaderModules.push_back(
             VulkanShaderModule(inDevice, inFragmentShaderSPIRV, false));
    }
    if (inComputeShaderSPIRV.size() > 0) {
        mComputeShaderModule.push_back(VulkanShaderModule(inDevice, inComputeShaderSPIRV, false));
    }
}

Jkr::ShaderModules::~ShaderModules() {
    for (auto &m : mVertexFragmentShaderModules) {
        if (m.GetShaderModuleHandle()) {
            mDevice.GetDeviceHandle().destroyShaderModule(m.GetShaderModuleHandle());
        }
    }
    for (auto &m : mComputeShaderModule) {
        if (m.GetShaderModuleHandle()) {
            mDevice.GetDeviceHandle().destroyShaderModule(m.GetShaderModuleHandle());
        }
    }
}

Jkr::PainterCache::PainterCache(Instance &inInstance, PipelinePropertiesContext inContext)
    : mInstance(inInstance), mPipelineContext(inContext) {}

Jkr::PainterCache &Jkr::PainterCache::Store(Jkr::Misc::File &inFile, const std::string_view inFileName,
                                            const std::string_view inVertexShader,
                                            const std::string_view inFragmentShader,
                                            const std::string_view inComputeShader,
                                            ui inVarDescount) {
    if(not inFile.HasEntry(s(inFileName) + "VS"))
    {
        ShaderCompiler Compiler(inVertexShader,
                                inFragmentShader,
                                mVertexFragmentShaderSPIRV[0],
                                mVertexFragmentShaderSPIRV[1]);
        ShaderCompiler ComputeCompiler(inComputeShader, mComputeShaderSPIRV[0]);
        mShaderModulesPtr = MakeUp<ShaderModules>(mInstance.GetDevice(),
                                                mVertexFragmentShaderSPIRV[0],
                                                mVertexFragmentShaderSPIRV[1],
                                                mComputeShaderSPIRV[0]);


        StoreSPIRVsToFile(inFile, inFileName);
    } else {
        LoadSPIRVsFromFile(inFile, inFileName);
        mShaderModulesPtr        = MakeUp<ShaderModules>(mInstance.GetDevice(),
                                                mVertexFragmentShaderSPIRV[0],
                                                mVertexFragmentShaderSPIRV[1],
                                                mComputeShaderSPIRV[0]);
    }
    if (mShaderModulesPtr->GetShaderModulesArray().size() != 0) {
        mPtrVertexFragmentDescriptorSetLayout =
            MakeUp<VulkanDescriptorSetLayout<2,
                                            ShaderModuleContext::Vertex,
                                            ShaderModuleContext::Fragment>>(
                mInstance.GetDevice(), mShaderModulesPtr->GetShaderModulesArray());
        mPtrVertexFragmentPipelineLayout =
            MakeUp<VulkanPipelineLayout<2>>(mInstance.GetDevice(),
                                            *mPtrVertexFragmentDescriptorSetLayout,
                                            mShaderModulesPtr->GetShaderModulesArray());
        mPtrPipelineCache = MakeUp<VulkanPipelineCache>(mInstance.GetDevice(), inFileName);
    }

    if (mShaderModulesPtr->GetComputeShaderModuleArray().size() != 0) {
        mPtrComputeDescriptorSetLayout =
            MakeUp<VulkanDescriptorSetLayout<1, ShaderModuleContext::Compute>>(
                mInstance.GetDevice(), mShaderModulesPtr->GetComputeShaderModuleArray());
        mPtrComputePipelineLayout =
            MakeUp<VulkanPipelineLayout<1>>(mInstance.GetDevice(),
                                            *mPtrComputeDescriptorSetLayout,
                                            mShaderModulesPtr->GetComputeShaderModuleArray());
    }
    return *this;
}

void Jkr::PainterCache::StoreSPIRVsToFile(Jkr::Misc::File& mCacheFile, const std::string_view inFileName) {
    if (not(mVertexFragmentShaderSPIRV[0].empty() or mVertexFragmentShaderSPIRV[1].empty())) {
            mCacheFile.Write((s(inFileName) + "VS"), mVertexFragmentShaderSPIRV[0]);
            mCacheFile.Write((s(inFileName) + "FS"), mVertexFragmentShaderSPIRV[1]);
            mCacheFile.Write((s(inFileName) + "CS"), mComputeShaderSPIRV[0]);
    }
}

void Jkr::PainterCache::LoadSPIRVsFromFile(Jkr::Misc::File& inFile, const std::string_view inFileName) {
    if (mVertexFragmentShaderSPIRV[0].empty() or mVertexFragmentShaderSPIRV[1].empty()) {
        mVertexFragmentShaderSPIRV[0] =
                inFile.Read<v<uint32_t>>((s(inFileName) + "VS"));
        mVertexFragmentShaderSPIRV[1] =
                inFile.Read<v<uint32_t>>((s(inFileName) + "FS"));
        mComputeShaderSPIRV[0] = inFile.Read<v<uint32_t>>((s(inFileName) + "CS"));
    }
}