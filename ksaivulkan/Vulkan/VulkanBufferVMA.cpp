#include "VulkanBufferVMA.hpp"
using namespace ksai;

void VulkanBufferVMA::MapMemoryRegion(void **outMappedMemoryRegion) {
    if (not mMemoryMapped) {
        vmaMapMemory(mAllocator->GetVMAHandle(), mAllocation, &mMappedMemoryRegion);
        mMemoryMapped = true;
    }
    *outMappedMemoryRegion = mMappedMemoryRegion;
}

void VulkanBufferVMA::UnMapMemoryRegion() {
    if (mMemoryMapped) {
        mMemoryMapped = false;
        vmaUnmapMemory(mAllocator->GetVMAHandle(), mAllocation);
    }
}

VulkanBufferVMA::VulkanBufferVMA(const VulkanVMA &inVMA,
                                 const VulkanDevice &inDevice,
                                 size_t inSize,
                                 BufferContext inBufferContext,
                                 MemoryType inBufferStorageType) {
    Init({&inVMA, &inDevice, inSize, inBufferContext, inBufferStorageType});
}

VulkanBufferVMA::~VulkanBufferVMA() {
    if (mInitialized) {
        Destroy();
    }
};

void VulkanBufferVMA::Init(CreateInfo info) {
    VulkanBufferBase::Init({info.inDevice});
    mAllocator            = info.inVMA;
    mSize                 = info.inSize;
    auto BufferCreateInfo = vk::BufferCreateInfo(vk::BufferCreateFlags(),
                                                 info.inSize,
                                                 vk::BufferUsageFlagBits::eVertexBuffer |
                                                      vk::BufferUsageFlagBits::eTransferDst);
    FillBufferUsage(BufferCreateInfo, info.inBufferContext, info.inBufferMemoryType);
    VkBufferCreateInfo BufferCreateInfo_C = BufferCreateInfo;

    auto AllocationCreateInfo_C           = VmaAllocationCreateInfo();
    AllocationCreateInfo_C.usage          = VMA_MEMORY_USAGE_AUTO;
    if (info.inBufferMemoryType == MemoryType::HostVisibleAndCoherenet)
        AllocationCreateInfo_C.flags =
             AllocationCreateInfo_C.flags | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    else if (info.inBufferMemoryType == MemoryType::HostVisibleAndHostCached) {
        AllocationCreateInfo_C.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        AllocationCreateInfo_C.requiredFlags =
             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
    }

    VkBuffer Buffer_C;
    vmaCreateBuffer(mAllocator->GetVMAHandle(),
                    &BufferCreateInfo_C,
                    &AllocationCreateInfo_C,
                    &Buffer_C,
                    &mAllocation,
                    nullptr);
    mBufferHandle = Buffer_C;
    mInitialized  = true;
}

void VulkanBufferVMA::Destroy() {
    if (mMemoryMapped) vmaUnmapMemory(mAllocator->GetVMAHandle(), mAllocation);
    mDevice->waitIdle();
    vmaDestroyBuffer(mAllocator->GetVMAHandle(), mBufferHandle, mAllocation);
    mInitialized = false;
}