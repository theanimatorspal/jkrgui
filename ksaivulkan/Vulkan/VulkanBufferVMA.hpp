#pragma once
#include <vma/vk_mem_alloc.h>
#include "VulkanBuffer.hpp"
#include "VulkanVMA.hpp"

namespace ksai {
	template<BufferContext inBufferContext, MemoryType inBufferStorageType>
	class VulkanBufferVMA : public VulkanBufferBase
	{
	public:
		inline constexpr void MapMemoryRegion(void** outMappedMemoryRegion) requires (inBufferStorageType == MemoryType::HostVisibleAndCoherenet || inBufferStorageType == MemoryType::HostVisibleAndHostCached);
		inline constexpr void UnMapMemoryRegion() requires (inBufferStorageType == MemoryType::HostVisibleAndCoherenet);
		inline constexpr void FlushMemoryRanges(vk::DeviceSize inOffset, vk::DeviceSize inSize) requires (inBufferStorageType == MemoryType::HostVisibleAndHostCached)
		{
			vmaFlushAllocation(mAllocator.GetVMAHandle(), mAllocation, inOffset, inSize);
		}
	public:
		VulkanBufferVMA(const VulkanVMA& inVMA, const VulkanDevice& inDevice, size_t inSize);
		~VulkanBufferVMA() {
			if constexpr (inBufferStorageType == MemoryType::HostVisibleAndCoherenet) vmaUnmapMemory(mAllocator.GetVMAHandle(), mAllocation);
			else if constexpr (inBufferStorageType == MemoryType::HostVisibleAndHostCached) vmaUnmapMemory(mAllocator.GetVMAHandle(), mAllocation);
			vmaDestroyBuffer(mAllocator.GetVMAHandle(), mBufferHandle, mAllocation);
		};
	private:
		const VulkanVMA& mAllocator;
		VmaAllocation mAllocation;
	};
}

namespace ksai {
	template<BufferContext inBufferContext, MemoryType inBufferStorageType>
	inline constexpr void VulkanBufferVMA<inBufferContext, inBufferStorageType>::MapMemoryRegion(void** outMappedMemoryRegion)  requires (inBufferStorageType == MemoryType::HostVisibleAndCoherenet || inBufferStorageType == MemoryType::HostVisibleAndHostCached)
	{
		vmaMapMemory(mAllocator.GetVMAHandle(), mAllocation, outMappedMemoryRegion);
	}
	template<BufferContext inBufferContext, MemoryType inBufferStorageType>
	inline constexpr void VulkanBufferVMA<inBufferContext, inBufferStorageType>::UnMapMemoryRegion() requires (inBufferStorageType == MemoryType::HostVisibleAndCoherenet)
	{
		vmaUnmapMemory(mAllocator.GetVMAHandle(), mAllocation);
	}
	template<BufferContext inBufferContext, MemoryType inBufferStorageType>
	inline VulkanBufferVMA<inBufferContext, inBufferStorageType>::VulkanBufferVMA(const VulkanVMA& inVMA, const VulkanDevice& inDevice, size_t inSize)
		: mAllocator(inVMA), VulkanBufferBase(inDevice)
	{
		mSize = inSize;
		auto BufferCreateInfo = vk::BufferCreateInfo(
			vk::BufferCreateFlags(),
			inSize,
			vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst
		);
		FillBufferUsage<inBufferContext, inBufferStorageType>(BufferCreateInfo);
		VkBufferCreateInfo BufferCreateInfo_C = BufferCreateInfo;

		auto AllocationCreateInfo_C = VmaAllocationCreateInfo();
		AllocationCreateInfo_C.usage = VMA_MEMORY_USAGE_AUTO;
		if constexpr (inBufferStorageType == MemoryType::HostVisibleAndCoherenet) AllocationCreateInfo_C.flags = AllocationCreateInfo_C.flags | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		else if constexpr (inBufferStorageType == MemoryType::HostVisibleAndHostCached)
		{
			AllocationCreateInfo_C.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
			AllocationCreateInfo_C.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		}

		VkBuffer Buffer_C;
		vmaCreateBuffer(mAllocator.GetVMAHandle(), &BufferCreateInfo_C, &AllocationCreateInfo_C, &Buffer_C, &mAllocation, nullptr);
		mBufferHandle = Buffer_C;
	}
}

