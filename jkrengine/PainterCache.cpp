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
        if (!success) std::cout << "VertexShader Failed=============================\n";
        success = SpirvHelper::GLSLtoSPV(vk::ShaderStageFlagBits::eFragment,
                                         inFragmentShaderString.data(),
                                         outFragmentShaderModule);
        if (!success) std::cout << "FragmentShader Failed=============================\n";
    }
}

ShaderCompiler::ShaderCompiler(const std::string_view inComputeShaderString,
                               std::vector<uint32_t> &outComputeShaderModule) {
    if (inComputeShaderString != "") {
        bool success = SpirvHelper::GLSLtoSPV(vk::ShaderStageFlagBits::eCompute,
                                              inComputeShaderString.data(),
                                              outComputeShaderModule);
        if (!success) std::cout << "ComputeShader Failed=============================\n";
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

/// @todo later implement inVardescount, for performance
Jkr::PainterCache &Jkr::PainterCache::Load(const std::string &fileName, ui inVardescount) {
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
        mPtrPipelineCache = MakeUp<VulkanPipelineCache>(mInstance.GetDevice(), fileName);
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
    time.elapsed("Shader Descriptor + Pipeline Reflection");

    return *this;
}

Jkr::PainterCache &Jkr::PainterCache::Store(const std::string_view fileName,
                                            const std::string_view inVertexShader,
                                            const std::string_view inFragmentShader,
                                            const std::string_view inComputeShader,
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

    if (mShaderModulesPtr->GetShaderModulesArray().size() != 0) {
        time.reset();
        mPtrVertexFragmentDescriptorSetLayout =
             MakeUp<VulkanDescriptorSetLayout<2,
                                              ShaderModuleContext::Vertex,
                                              ShaderModuleContext::Fragment>>(
                  mInstance.GetDevice(), mShaderModulesPtr->GetShaderModulesArray());
        mPtrVertexFragmentPipelineLayout =
             MakeUp<VulkanPipelineLayout<2>>(mInstance.GetDevice(),
                                             *mPtrVertexFragmentDescriptorSetLayout,
                                             mShaderModulesPtr->GetShaderModulesArray());
        mPtrPipelineCache = MakeUp<VulkanPipelineCache>(mInstance.GetDevice(), fileName);
    }

    if (mShaderModulesPtr->GetComputeShaderModuleArray().size() != 0) {
        mPtrComputeDescriptorSetLayout =
             MakeUp<VulkanDescriptorSetLayout<1, ShaderModuleContext::Compute>>(
                  mInstance.GetDevice(), mShaderModulesPtr->GetComputeShaderModuleArray());
        mPtrComputePipelineLayout =
             MakeUp<VulkanPipelineLayout<1>>(mInstance.GetDevice(),
                                             *mPtrComputeDescriptorSetLayout,
                                             mShaderModulesPtr->GetComputeShaderModuleArray());
        time.elapsed("Shader Descriptor + Pipeline Reflection");
    }

    StoreSPIRVsToFile(fileName);

    return *this;
}

static void CreateIfDoesntExist(const std::string_view inFileName) {
    auto Directory = inFileName.substr(0, inFileName.find_last_of('/'));
    if (not std::filesystem::exists(Directory)) {
        std::filesystem::create_directory(Directory);
    }
}

void Jkr::PainterCache::StoreSPIRVsToFile(const std::string_view inFileName) {
    if (not(mVertexFragmentShaderSPIRV[0].empty() or mVertexFragmentShaderSPIRV[1].empty())) {
        if (not mCacheFile) {
            CreateIfDoesntExist(inFileName);
            std::ofstream Fstream(std::string(inFileName), std::ios_base::binary);
            uint32_t size_V = mVertexFragmentShaderSPIRV[0].size() * sizeof(uint32_t);
            uint32_t size_F = mVertexFragmentShaderSPIRV[1].size() * sizeof(uint32_t);
            uint32_t size_C = mComputeShaderSPIRV[0].size() * sizeof(uint32_t);
            Fstream.write(reinterpret_cast<char *>(&size_V), sizeof(uint32_t));
            Fstream.write(reinterpret_cast<char *>(&size_F), sizeof(uint32_t));
            Fstream.write(reinterpret_cast<char *>(&size_C), sizeof(uint32_t));
            Fstream.write(reinterpret_cast<char *>(mVertexFragmentShaderSPIRV[0].data()),
                          mVertexFragmentShaderSPIRV[0].size() * sizeof(uint32_t));
            Fstream.write(reinterpret_cast<char *>(mVertexFragmentShaderSPIRV[1].data()),
                          mVertexFragmentShaderSPIRV[1].size() * sizeof(uint32_t));
            Fstream.write(reinterpret_cast<char *>(mComputeShaderSPIRV[0].data()),
                          mComputeShaderSPIRV[0].size() * sizeof(uint32_t));
        } else {
            mCacheFile->Write((s(inFileName) + "VS").c_str(), mVertexFragmentShaderSPIRV[0]);
            mCacheFile->Write((s(inFileName) + "FS").c_str(), mVertexFragmentShaderSPIRV[1]);
            mCacheFile->Write((s(inFileName) + "CS").c_str(), mComputeShaderSPIRV[0]);
        }
    }
    mCacheFileEntryName = s(inFileName);
}

void Jkr::PainterCache::LoadSPIRVsFromFile(const std::string_view inFileName) {
    ///@note if mVertexFragmentShaderSPIRV[0] size is > 0 means it has already been loaded with
    /// other means (possibly a JkrFile)
    if (mVertexFragmentShaderSPIRV[0].empty() or mVertexFragmentShaderSPIRV[1].empty()) {
        if (not mCacheFile) {
            if (std::filesystem::exists(inFileName)) {
                std::ifstream Fstream(std::string(inFileName), std::ios_base::binary);
                uint32_t size_V = 0;
                uint32_t size_F = 0;
                uint32_t size_C = 0;
                Fstream.read(reinterpret_cast<char *>(&size_V), sizeof(uint32_t));
                Fstream.read(reinterpret_cast<char *>(&size_F), sizeof(uint32_t));
                Fstream.read(reinterpret_cast<char *>(&size_C), sizeof(uint32_t));

                mVertexFragmentShaderSPIRV[0].resize(size_V / sizeof(uint32_t));
                mVertexFragmentShaderSPIRV[1].resize(size_F / sizeof(uint32_t));
                mComputeShaderSPIRV[0].resize(size_C / sizeof(uint32_t));
                Fstream.read(reinterpret_cast<char *>(mVertexFragmentShaderSPIRV[0].data()),
                             mVertexFragmentShaderSPIRV[0].size() * sizeof(uint32_t));
                Fstream.read(reinterpret_cast<char *>(mVertexFragmentShaderSPIRV[1].data()),
                             mVertexFragmentShaderSPIRV[1].size() * sizeof(uint32_t));
                Fstream.read(reinterpret_cast<char *>(mComputeShaderSPIRV[0].data()),
                             mComputeShaderSPIRV[0].size() * sizeof(uint32_t));
            }
        } else {
            mVertexFragmentShaderSPIRV[0] =
                 mCacheFile->Read<v<uint32_t>>((s(inFileName) + "VS").c_str());
            mVertexFragmentShaderSPIRV[1] =
                 mCacheFile->Read<v<uint32_t>>((s(inFileName) + "FS").c_str());
            mComputeShaderSPIRV[0] = mCacheFile->Read<v<uint32_t>>((s(inFileName) + "CS").c_str());
        }
    }
    mCacheFileEntryName = s(inFileName);
}

void Jkr::PainterCache::WriteToCacheFile(Jkr::Misc::FileJkr &inFileJkr, std::string inEntry) {
    inFileJkr.Write((inEntry + "VS").c_str(), mVertexFragmentShaderSPIRV[0]);
    inFileJkr.Write((inEntry + "FS").c_str(), mVertexFragmentShaderSPIRV[1]);
    inFileJkr.Write((inEntry + "CS").c_str(), mComputeShaderSPIRV[0]);
    mCacheFileEntryName = inEntry;
}
void Jkr::PainterCache::ReadFromCacheFile(Jkr::Misc::FileJkr &inFileJkr, std::string inEntry) {
    mVertexFragmentShaderSPIRV[0] = inFileJkr.Read<v<uint32_t>>((inEntry + "VS").c_str());
    mVertexFragmentShaderSPIRV[1] = inFileJkr.Read<v<uint32_t>>((inEntry + "FS").c_str());
    mComputeShaderSPIRV[0]        = inFileJkr.Read<v<uint32_t>>((inEntry + "CS").c_str());
    mCacheFileEntryName           = inEntry;
}