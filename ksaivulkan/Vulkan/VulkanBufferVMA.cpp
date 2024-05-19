#include "VulkanBufferVMA.hpp"
using namespace ksai;

void VulkanBufferVMA::MapMemoryRegion(void** outMappedMemoryRegion) {
    mMemoryMapped = true;
    vmaMapMemory(mAllocator.GetVMAHandle(), mAllocation, outMappedMemoryRegion);
}

void VulkanBufferVMA::UnMapMemoryRegion() {
    vmaUnmapMemory(mAllocator.GetVMAHandle(), mAllocation);
}

VulkanBufferVMA::VulkanBufferVMA(const VulkanVMA& inVMA,
                                 const VulkanDevice& inDevice,
                                 size_t inSize,
                                 BufferContext inBufferContext,
                                 MemoryType inBufferStorageType)
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
    if (inBufferStorageType == MemoryType::HostVisibleAndCoherenet)
        AllocationCreateInfo_C.flags =
             AllocationCreateInfo_C.flags | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    else if (inBufferStorageType == MemoryType::HostVisibleAndHostCached) {
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