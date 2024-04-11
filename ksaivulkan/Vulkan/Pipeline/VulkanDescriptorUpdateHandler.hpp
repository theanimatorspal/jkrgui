#pragma once
#include <Vulkan/Pipeline/VulkanDescriptorSet.hpp>
#include <Vulkan/VulkanDevice.hpp>
#include <Vulkan/VulkanSampler.hpp>
#include <VulkanBuffer.hpp>

namespace ksai {
class VulkanDescriptorUpdateHandler {
      public:
          VulkanDescriptorUpdateHandler(const VulkanDevice& inDevice) : mDevice(inDevice) {}
          template <BufferContext inBufferContext>
          void RW(const VulkanDescriptorSet& inDescriptorSet,
                  const VulkanBufferBase& inBuffer,
                  vk::DeviceSize inOffset,
                  uint32_t inDstBinding,
                  uint32_t inDstArrayElement) {
                    auto BufferInfo = vk::DescriptorBufferInfo(inBuffer.GetBufferHandle(), inOffset, inBuffer.GetBufferSize());

                    auto DescriptorType = vk::DescriptorType::eStorageBuffer;
                    if constexpr (inBufferContext == BufferContext::Storage)
                              DescriptorType = vk::DescriptorType::eStorageBuffer;
                    else if constexpr (inBufferContext == BufferContext::Uniform)
                              DescriptorType = vk::DescriptorType::eUniformBuffer;

                    auto DescriptorSetWrite = vk::WriteDescriptorSet()
                                               .setDescriptorCount(1)
                                               .setDstBinding(inDstBinding)
                                               .setDstArrayElement(inDstArrayElement)
                                               .setBufferInfo(BufferInfo)
                                               .setDescriptorType(DescriptorType)
                                               .setDstSet(inDescriptorSet.GetDescriptorSetHandle());

                    mDescriptorWrites.push_back(DescriptorSetWrite);
                    mDevice.GetDeviceHandle().updateDescriptorSets(mDescriptorWrites, {});
                    mDescriptorWrites.clear();
          }

          template <ImageContext inImageContext>
          void RW(const VulkanDescriptorSet inDescriptorSet,
                  const VulkanImageBase& inImage,
                  const VulkanSampler& inSampler,
                  uint32_t inDstBinding,
                  uint32_t inDstArrayElement) {
                    auto imageinfo =
                     vk::DescriptorImageInfo(inSampler.GetSamplerHandle(), inImage.GetImageViewHandle(), inImage.GetInitialImageLayout());
                    auto DescriptorType = vk::DescriptorType::eCombinedImageSampler;
                    if constexpr (inImageContext == ImageContext::Storage) DescriptorType = vk::DescriptorType::eStorageImage;

                    auto DescriptorSetWrite = vk::WriteDescriptorSet()
                                               .setDescriptorCount(1)
                                               .setDstBinding(inDstBinding)
                                               .setDstArrayElement(inDstArrayElement)
                                               .setImageInfo(imageinfo)
                                               .setDescriptorType(DescriptorType)
                                               .setDstSet(inDescriptorSet.GetDescriptorSetHandle());

                    mDescriptorWrites.push_back(DescriptorSetWrite);
                    mDevice.GetDeviceHandle().updateDescriptorSets(mDescriptorWrites, {});
                    mDescriptorWrites.clear();
          }

          void RW(const VulkanDescriptorSet& inDescriptorSet, const VulkanSampler& inSampler, uint32_t inDstBinding, uint32_t inDstArrayElement) {
                    auto imageInfo = vk::DescriptorImageInfo(inSampler.GetSamplerHandle());
                    auto DescriptorType = vk::DescriptorType::eSampler;
                    auto DescriptorSetWrite = vk::WriteDescriptorSet()
                                               .setDescriptorCount(1)
                                               .setDstBinding(inDstBinding)
                                               .setDescriptorType(DescriptorType)
                                               .setDstArrayElement(inDstArrayElement)
                                               .setDstSet(inDescriptorSet.GetDescriptorSetHandle())
                                               .setImageInfo(imageInfo);
                    mDescriptorWrites.push_back(DescriptorSetWrite);
                    mDevice.GetDeviceHandle().updateDescriptorSets(mDescriptorWrites, {});
                    mDescriptorWrites.push_back(DescriptorSetWrite);
                    mDevice.GetDeviceHandle().updateDescriptorSets(mDescriptorWrites, {});
                    mDescriptorWrites.clear();
          }

      private:
          const VulkanDevice& mDevice;
          std::vector<vk::WriteDescriptorSet> mDescriptorWrites;
};
} // namespace ksai

namespace ksai {}
