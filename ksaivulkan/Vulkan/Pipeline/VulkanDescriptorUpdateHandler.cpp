#include "VulkanDescriptorUpdateHandler.hpp"
using namespace ksai;
static std::mutex DescMutex;
void VulkanDescriptorUpdateHandler::RW(BufferContext inBufferContext,
                                       const VulkanDescriptorSet& inDescriptorSet,
                                       const VulkanBufferBase& inBuffer,
                                       vk::DeviceSize inOffset,
                                       uint32_t inDstBinding,
                                       uint32_t inDstArrayElement,
                                       uint32_t inDstSet) {
    auto BufferInfo =
         vk::DescriptorBufferInfo(inBuffer.GetBufferHandle(), inOffset, inBuffer.GetBufferSize());

    auto DescriptorType = vk::DescriptorType::eStorageBuffer;
    if (inBufferContext == BufferContext::Storage)
        DescriptorType = vk::DescriptorType::eStorageBuffer;
    else if (inBufferContext == BufferContext::Uniform)
        DescriptorType = vk::DescriptorType::eUniformBuffer;

    auto DescriptorSetWrite = vk::WriteDescriptorSet()
                                   .setDescriptorCount(1)
                                   .setDstBinding(inDstBinding)
                                   .setDstArrayElement(inDstArrayElement)
                                   .setBufferInfo(BufferInfo)
                                   .setDescriptorType(DescriptorType)
                                   .setDstSet(inDescriptorSet.GetDescriptorSetHandle()[inDstSet]);

    std::lock_guard<std::mutex> Lock(DescMutex);
    mDescriptorWrites.push_back(DescriptorSetWrite);
    mDevice.GetDeviceHandle().updateDescriptorSets(mDescriptorWrites, {});
    mDescriptorWrites.clear();
}

void VulkanDescriptorUpdateHandler::RW(ImageContext inImageContext,
                                       const VulkanDescriptorSet& inDescriptorSet,
                                       const VulkanImageBase& inImage,
                                       const VulkanSampler& inSampler,
                                       uint32_t inDstBinding,
                                       uint32_t inDstArrayElement,
                                       uint32_t inDstSet) {
    auto imageinfo      = vk::DescriptorImageInfo(inSampler.GetSamplerHandle(),
                                             inImage.GetImageViewHandle(),
                                             inImage.GetInitialImageLayout());
    auto DescriptorType = vk::DescriptorType::eCombinedImageSampler;
    if (inImageContext == ImageContext::Storage) DescriptorType = vk::DescriptorType::eStorageImage;

    auto DescriptorSetWrite = vk::WriteDescriptorSet()
                                   .setDescriptorCount(1)
                                   .setDstBinding(inDstBinding)
                                   .setDstArrayElement(inDstArrayElement)
                                   .setImageInfo(imageinfo)
                                   .setDescriptorType(DescriptorType)
                                   .setDstSet(inDescriptorSet.GetDescriptorSetHandle()[inDstSet]);

    std::lock_guard<std::mutex> Lock(DescMutex);
    mDescriptorWrites.push_back(DescriptorSetWrite);
    mDevice.GetDeviceHandle().updateDescriptorSets(mDescriptorWrites, {});
    mDescriptorWrites.clear();
}

void VulkanDescriptorUpdateHandler::RW(const VulkanDescriptorSet& inDescriptorSet,
                                       const VulkanSampler& inSampler,
                                       uint32_t inDstBinding,
                                       uint32_t inDstArrayElement,
                                       uint32_t inDstSet) {
    auto imageInfo          = vk::DescriptorImageInfo(inSampler.GetSamplerHandle());
    auto DescriptorType     = vk::DescriptorType::eSampler;
    auto DescriptorSetWrite = vk::WriteDescriptorSet()
                                   .setDescriptorCount(1)
                                   .setDstBinding(inDstBinding)
                                   .setDescriptorType(DescriptorType)
                                   .setDstArrayElement(inDstArrayElement)
                                   .setDstSet(inDescriptorSet.GetDescriptorSetHandle()[inDstSet])
                                   .setImageInfo(imageInfo);
    mDescriptorWrites.push_back(DescriptorSetWrite); // TODO WTF is this
    mDevice.GetDeviceHandle().updateDescriptorSets(mDescriptorWrites, {});

    std::lock_guard<std::mutex> Lock(DescMutex);
    // mDescriptorWrites.push_back(DescriptorSetWrite);
    // mDevice.GetDeviceHandle().updateDescriptorSets(mDescriptorWrites, {});
    mDescriptorWrites.clear();
}