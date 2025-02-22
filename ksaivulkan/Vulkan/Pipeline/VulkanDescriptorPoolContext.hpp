#pragma once
#include <Vulkan/VulkanDevice.hpp>

namespace ksai {
enum class DescriptorType {
    UniformBuffer,
    StorageBuffer,
    Image
};

constexpr auto DescriptorPoolArraySize = 11;

}

namespace ksai {
class VulkanDescriptorPoolContext {
    const std::array<std::pair<vk::DescriptorType, float>, DescriptorPoolArraySize> PoolItemsWithPercent = { 
        { { vk::DescriptorType::eSampler, 0.5f },
        { vk::DescriptorType::eCombinedImageSampler, 10.f },
        { vk::DescriptorType::eSampledImage, 1.f },
        { vk::DescriptorType::eStorageImage, 10.f },
        { vk::DescriptorType::eUniformTexelBuffer, 1.f },
        { vk::DescriptorType::eStorageTexelBuffer, 1.f },
        { vk::DescriptorType::eUniformBuffer, 10.f },
        { vk::DescriptorType::eStorageBuffer, 4.f },
        { vk::DescriptorType::eUniformBufferDynamic, 1.f },
        { vk::DescriptorType::eStorageBufferDynamic, 1.f },
        { vk::DescriptorType::eInputAttachment, 0.5f } } };

public:
    constexpr VulkanDescriptorPoolContext(uint32_t inPoolSize)
        : PoolSize(inPoolSize)
    {
        for (int i = 0; i < DescriptorPoolArraySize; i++) {
            mDescriptorPoolSizesArray[i] = vk::DescriptorPoolSize(PoolItemsWithPercent[i].first, PoolItemsWithPercent[i].second * PoolSize);
        }
    }
    GETTER& GetDescriptorPoolSizesArray() const { return mDescriptorPoolSizesArray; }

private:
    std::array<vk::DescriptorPoolSize, DescriptorPoolArraySize> mDescriptorPoolSizesArray;
    uint32_t PoolSize;
};
}
