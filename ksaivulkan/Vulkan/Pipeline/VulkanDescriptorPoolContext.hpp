#pragma once
#include <Vulkan/VulkanDevice.hpp>

namespace ksai {
	enum class DescriptorType
	{
		UniformBuffer,
		StorageBuffer,
		Image
	};

	constexpr auto DescriptorPoolArraySize = 11;

}

namespace ksai {
	template<size_t PoolSize>
	class VulkanDescriptorPoolContext
	{
			const std::array<std::pair<vk::DescriptorType, float>, DescriptorPoolArraySize> PoolItemsWithPercent = { {
			    { vk::DescriptorType::eSampler, 0.5f },
			    { vk::DescriptorType::eCombinedImageSampler, 4.f },
			    { vk::DescriptorType::eSampledImage, 4.f },
			    { vk::DescriptorType::eStorageImage, 1.f },
			    { vk::DescriptorType::eUniformTexelBuffer, 1.f },
			    { vk::DescriptorType::eStorageTexelBuffer, 1.f },
			    { vk::DescriptorType::eUniformBuffer, 2.f },
			    { vk::DescriptorType::eStorageBuffer, 2.f },
			    { vk::DescriptorType::eUniformBufferDynamic, 1.f },
			    { vk::DescriptorType::eStorageBufferDynamic, 1.f },
			    { vk::DescriptorType::eInputAttachment, 0.5f }
			} };
	public:
		constexpr VulkanDescriptorPoolContext()
		{
			for (int i = 0; i < DescriptorPoolArraySize; i++)
			{
				mDescriptorPoolSizesArray[i] = vk::DescriptorPoolSize(PoolItemsWithPercent[i].first, PoolItemsWithPercent[i].second * PoolSize);
			}
		}
		GETTER& GetDescriptorPoolSizesArray() const { return mDescriptorPoolSizesArray; }
	private:
		std::array<vk::DescriptorPoolSize, DescriptorPoolArraySize> mDescriptorPoolSizesArray;
	};
}
