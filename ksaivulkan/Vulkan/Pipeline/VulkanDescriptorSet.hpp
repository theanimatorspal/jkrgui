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
        VulkanDevice *inDevice                               = nullptr;
        VulkanDescriptorPoolBase *inDescriptorPool           = nullptr;
        VulkanDescriptorSetLayoutBase *inDescriptorSetLayout = nullptr;
        opt<uint32_t> inNoOfVariableDescriptorSets           = std::nullopt;
    };

    VulkanDescriptorSet()                                       = default;
    ~VulkanDescriptorSet()                                      = default;
    VulkanDescriptorSet(VulkanDescriptorSet &other)             = delete;
    VulkanDescriptorSet &operator=(VulkanDescriptorSet &other)  = delete;
    VulkanDescriptorSet(VulkanDescriptorSet &&other)            = default;
    VulkanDescriptorSet &operator=(VulkanDescriptorSet &&other) = default;

    void Init(CreateInfo inCreateInfo);
    void Bind(vk::PipelineBindPoint inBindPoint,
              VulkanCommandBuffer &inBuffer,
              VulkanPipelineLayoutBase &inBase,
              int inSet = 0);
    GETTER &GetDescriptorSetHandle() const { return mVulkanDescriptorSetHandles; }

    VulkanDescriptorSet(VulkanDevice &inDevice,
                        VulkanDescriptorPoolBase &inDescriptorPool,
                        VulkanDescriptorSetLayoutBase &inDescriptorSetLayout);
    VulkanDescriptorSet(VulkanDevice &inDevice,
                        VulkanDescriptorPoolBase &inDescriptorPool,
                        VulkanDescriptorSetLayoutBase &inDescriptorSetLayout,
                        uint32_t inNoOfVarDescriptorSets);

    private:
    vk::Device *mDevice;
    VulkanDescriptorPoolBase *mDescriptorPool;
    std::vector<vk::DescriptorSet> mVulkanDescriptorSetHandles;
    bool mInitialized = false;
};

} // namespace ksai
