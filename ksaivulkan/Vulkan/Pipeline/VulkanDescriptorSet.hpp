#pragma once
#include "Pipeline/VulkanPipelineLayout.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanDescriptorPool.hpp"
#include "VulkanDescriptorSetLayout.hpp"
#include "vulkan/vulkan_enums.hpp"
#include <Vulkan/VulkanBuffer.hpp>
#include <Vulkan/VulkanDevice.hpp>
#include <vector>

namespace ksai {
class VulkanDescriptorSet {
    public:
    struct CreateInfo {
        const VulkanDevice *inDevice                               = nullptr;
        const VulkanDescriptorPoolBase *inDescriptorPool           = nullptr;
        const VulkanDescriptorSetLayoutBase *inDescriptorSetLayout = nullptr;
        opt<uint32_t> inNoOfVariableDescriptorSets                 = std::nullopt;
    };

    VulkanDescriptorSet()                                            = default;
    ~VulkanDescriptorSet()                                           = default;
    VulkanDescriptorSet(const VulkanDescriptorSet &other)            = delete;
    VulkanDescriptorSet &operator=(const VulkanDescriptorSet &other) = delete;
    VulkanDescriptorSet(VulkanDescriptorSet &&other)                 = default;
    VulkanDescriptorSet &operator=(VulkanDescriptorSet &&other)      = default;

    void Init(CreateInfo inCreateInfo);
    void Bind(vk::PipelineBindPoint inBindPoint,
              const VulkanCommandBuffer &inBuffer,
              const VulkanPipelineLayoutBase &inBase,
              int inSet = 0);
    GETTER &GetDescriptorSetHandle() const { return mVulkanDescriptorSetHandles; }

    VulkanDescriptorSet(const VulkanDevice &inDevice,
                        const VulkanDescriptorPoolBase &inDescriptorPool,
                        const VulkanDescriptorSetLayoutBase &inDescriptorSetLayout);
    VulkanDescriptorSet(const VulkanDevice &inDevice,
                        const VulkanDescriptorPoolBase &inDescriptorPool,
                        const VulkanDescriptorSetLayoutBase &inDescriptorSetLayout,
                        uint32_t inNoOfVarDescriptorSets);

    private:
    const vk::Device *mDevice;
    const VulkanDescriptorPoolBase *mDescriptorPool;
    std::vector<vk::DescriptorSet> mVulkanDescriptorSetHandles;
    bool mInitialized = false;
};

} // namespace ksai
