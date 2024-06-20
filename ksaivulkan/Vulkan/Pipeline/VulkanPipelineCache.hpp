#pragma once
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <Vulkan/VulkanDevice.hpp>

namespace ksai {
class VulkanPipelineCache {
    public:
    VulkanPipelineCache(const VulkanDevice& inDevice, std::string_view inFileName);
    ~VulkanPipelineCache();
    GETTER& GetPipelineCacheHandle() const { return mPipelineCache; }
    std::vector<uint8_t> Get();
    void Load();

    private:
    std::string mName;
    const vk::Device& mDevice;
    vk::PipelineCache mPipelineCache;

    private:
    void* mPipelineCacheData  = nullptr;
    size_t mPipelineCacheSize = 0;

    private:
    size_t GetFileSizeUtil(FILE* inFile);
    void CheckCacheData(std::string& inFileName);
};
} // namespace ksai
