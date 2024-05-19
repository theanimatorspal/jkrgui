#pragma once
#include "VulkanBuffer.hpp"
#include "VulkanVMA.hpp"
#include <Vulkan/Config.hpp>
#include <vma/vk_mem_alloc.h>

namespace ksai {
class VulkanBufferVMA : public VulkanBufferBase {
    public:
    void MapMemoryRegion(void** outMappedMemoryRegion);
    void UnMapMemoryRegion();
    void FlushMemoryRanges(vk::DeviceSize inOffset, vk::DeviceSize inSize) {
        vmaFlushAllocation(mAllocator.GetVMAHandle(), mAllocation, inOffset, inSize);
    }

    public:
    VulkanBufferVMA(const VulkanVMA& inVMA,
                    const VulkanDevice& inDevice,
                    size_t inSize,
                    BufferContext inBufferContext,
                    MemoryType inBufferStorageType);
    ~VulkanBufferVMA() {
        if (mMemoryMapped) vmaUnmapMemory(mAllocator.GetVMAHandle(), mAllocation);
        mDevice.waitIdle();
        vmaDestroyBuffer(mAllocator.GetVMAHandle(), mBufferHandle, mAllocation);
    };

    private:
    const VulkanVMA& mAllocator;
    VmaAllocation mAllocation;
    bool mMemoryMapped = false;
};
} // namespace ksai
