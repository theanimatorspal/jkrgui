#pragma once
#include "VulkanBuffer.hpp"
#include "VulkanVMA.hpp"
#include <Vulkan/Config.hpp>
#include <vma/vk_mem_alloc.h>

namespace ksai {
template <BufferContext inBufferContext, MemoryType inBufferStorageType>
class VulkanBufferVMA : public VulkanBufferBase {
    public:
    SETTER MapMemoryRegion(void** outMappedMemoryRegion);
    SETTER UnMapMemoryRegion();
    SETTER FlushMemoryRanges(vk::DeviceSize inOffset, vk::DeviceSize inSize)
        requires(inBufferStorageType == MemoryType::HostVisibleAndHostCached)
    {
        vmaFlushAllocation(mAllocator.GetVMAHandle(), mAllocation, inOffset, inSize);
    }

    public:
    VulkanBufferVMA(const VulkanVMA& inVMA, const VulkanDevice& inDevice, size_t inSize);
    ~VulkanBufferVMA() {
        if constexpr (inBufferStorageType == MemoryType::HostVisibleAndCoherenet or
                      inBufferStorageType == MemoryType::HostVisibleAndHostCached) {
            if (mMemoryMapped) vmaUnmapMemory(mAllocator.GetVMAHandle(), mAllocation);
        }
        mDevice.waitIdle();
        vmaDestroyBuffer(mAllocator.GetVMAHandle(), mBufferHandle, mAllocation);
    };

    private:
    const VulkanVMA& mAllocator;
    VmaAllocation mAllocation;
    bool mMemoryMapped = false;
};
} // namespace ksai

namespace ksai {
template <BufferContext inBufferContext, MemoryType inBufferStorageType>
SETTER VulkanBufferVMA<inBufferContext, inBufferStorageType>::MapMemoryRegion(
     void** outMappedMemoryRegion) {
    mMemoryMapped = true;
    vmaMapMemory(mAllocator.GetVMAHandle(), mAllocation, outMappedMemoryRegion);
}
template <BufferContext inBufferContext, MemoryType inBufferStorageType>
SETTER VulkanBufferVMA<inBufferContext, inBufferStorageType>::UnMapMemoryRegion() {
    vmaUnmapMemory(mAllocator.GetVMAHandle(), mAllocation);
}
template <BufferContext inBufferContext, MemoryType inBufferStorageType>
inline VulkanBufferVMA<inBufferContext, inBufferStorageType>::VulkanBufferVMA(
     const VulkanVMA& inVMA, const VulkanDevice& inDevice, size_t inSize)
    : VulkanBufferBase(inDevice), mAllocator(inVMA) {
    mSize                 = inSize;
    auto BufferCreateInfo = vk::BufferCreateInfo(vk::BufferCreateFlags(),
                                                 inSize,
                                                 vk::BufferUsageFlagBits::eVertexBuffer |
                                                      vk::BufferUsageFlagBits::eTransferDst);
    FillBufferUsage(BufferCreateInfo, inBufferContext, inBufferStorageType);
    VkBufferCreateInfo BufferCreateInfo_C = BufferCreateInfo;

    auto AllocationCreateInfo_C           = VmaAllocationCreateInfo();
    AllocationCreateInfo_C.usage          = VMA_MEMORY_USAGE_AUTO;
    if constexpr (inBufferStorageType == MemoryType::HostVisibleAndCoherenet)
        AllocationCreateInfo_C.flags =
             AllocationCreateInfo_C.flags | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    else if constexpr (inBufferStorageType == MemoryType::HostVisibleAndHostCached) {
        AllocationCreateInfo_C.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        AllocationCreateInfo_C.requiredFlags =
             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
    }

    VkBuffer Buffer_C;
    vmaCreateBuffer(mAllocator.GetVMAHandle(),
                    &BufferCreateInfo_C,
                    &AllocationCreateInfo_C,
                    &Buffer_C,
                    &mAllocation,
                    nullptr);
    mBufferHandle = Buffer_C;
}
} // namespace ksai
