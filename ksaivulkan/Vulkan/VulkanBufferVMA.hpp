#pragma once
#include "VulkanBuffer.hpp"
#include "VulkanVMA.hpp"
#include <Vulkan/Config.hpp>
#include <vma/vk_mem_alloc.h>

namespace ksai {
class VulkanBufferVMA : public VulkanBufferBase {
    public:
    struct CreateInfo {
        VulkanVMA *inVMA;
        VulkanDevice *inDevice;
        size_t inSize;
        BufferContext inBufferContext;
        MemoryType inBufferMemoryType;
    };

    VulkanBufferVMA() = default;
    ~VulkanBufferVMA();
    VulkanBufferVMA(VulkanBufferVMA &other)             = delete;
    VulkanBufferVMA &operator=(VulkanBufferVMA &other)  = delete;
    VulkanBufferVMA(VulkanBufferVMA &&other)            = default;
    VulkanBufferVMA &operator=(VulkanBufferVMA &&other) = default;

    void Init(CreateInfo inCreateInfo);
    void Destroy();
    vk::Buffer operator()(VulkanBufferBase &) { return mBufferHandle; }

    void MapMemoryRegion(void **outMappedMemoryRegion);
    void UnMapMemoryRegion();
    void FlushMemoryRanges(vk::DeviceSize inOffset, vk::DeviceSize inSize) {
        vmaFlushAllocation(mAllocator->GetVMAHandle(), mAllocation, inOffset, inSize);
    }

    public:
    VulkanBufferVMA(VulkanVMA &inVMA,
                    VulkanDevice &inDevice,
                    size_t inSize,
                    BufferContext inBufferContext,
                    MemoryType inBufferStorageType);

    private:
    VulkanVMA *mAllocator;
    VmaAllocation mAllocation;
    bool mMemoryMapped        = false;
    bool mInitialized         = false;
    void *mMappedMemoryRegion = nullptr;
};
} // namespace ksai
