#pragma once
#include "VulkanCommandBuffer.hpp"
#include "VulkanPipelineCache.hpp"
#include "VulkanPipelineContext.hpp"
#include "VulkanPipelineLayout.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanShaderModule.hpp"

namespace ksai {

class VulkanPipelineBase {
    public:
    struct CreateInfo {
        const VulkanDevice *inDevice;
    };

    VulkanPipelineBase() = default;
    ~VulkanPipelineBase();
    VulkanPipelineBase(const VulkanPipelineBase &other)            = delete;
    VulkanPipelineBase &operator=(const VulkanPipelineBase &other) = delete;
    VulkanPipelineBase(VulkanPipelineBase &&other)                 = default;
    VulkanPipelineBase &operator=(VulkanPipelineBase &&other)      = default;
    operator vk::Pipeline() const { return mPipeline; }

    void Init(CreateInfo inCreateInfo);
    void Destroy();
    GETTER &GetPipelineHandle() const { return mPipeline; }

    void BuildPipeline(VulkanPipelineCache &inCache,
                       const VulkanPipelineContextBase &inContext,
                       const VulkanRenderPassBase &inRenderPass,
                       const VulkanPipelineLayoutBase &inLayout,
                       const std::vector<VulkanShaderModule> &inModules,
                       PipelineContext inPipelineContext,
                       ui inSubpass = 0);

    template <PipelineContext inContext> void Bind(const VulkanCommandBuffer &inCmdBuffer) const;
    void DrawIndexed(const VulkanCommandBuffer &inCmdBuffer,
                     int32_t inIndexCount,
                     int32_t inInstanceCount,
                     int32_t inFirstIndex,
                     int32_t inVertexOffset,
                     int32_t inFirstInstance) const;
    void FillVertexInputDescriptions(const spirv_cross::ShaderResources &Resources,
                                     const spirv_cross::Compiler &comp,
                                     std::vector<vk::VertexInputBindingDescription> &VertexInputBindingDesp,
                                     std::vector<vk::VertexInputAttributeDescription> &InputAttrDescription);

    VulkanPipelineBase(const VulkanDevice &inDevice);

    protected:
    const vk::Device *mDevice;
    vk::Pipeline mPipeline;
    bool mInitialized = false;
};

// TODO This class is only for backwards compatibility
template <size_t NoOfShaderModules, PipelineContext inPipelineContext> class VulkanPipeline : public VulkanPipelineBase {
    public:
    struct CreateInfo {};

    VulkanPipeline()                                       = default;
    ~VulkanPipeline()                                      = default;
    VulkanPipeline(const VulkanPipeline &other)            = delete;
    VulkanPipeline &operator=(const VulkanPipeline &other) = delete;
    VulkanPipeline(VulkanPipeline &&other)                 = default;
    VulkanPipeline &operator=(VulkanPipeline &&other)      = default;
    operator vk::Pipeline() const { return mPipeline; }

    void Init(CreateInfo inCreateInfo);
    void Destroy();

    VulkanPipeline(const VulkanDevice &inDevice,
                   VulkanPipelineCache &inCache,
                   const VulkanPipelineContextBase &inContext,
                   const VulkanRenderPassBase &inRenderPass,
                   const VulkanPipelineLayoutBase &inLayout,
                   const std::vector<VulkanShaderModule> &inModules);
};

template <PipelineContext inContext> inline void VulkanPipelineBase::Bind(const VulkanCommandBuffer &inCmdBuffer) const {
    if constexpr (inContext == PipelineContext::Default)
        inCmdBuffer.GetCommandBufferHandle().bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);
    else if constexpr (inContext == PipelineContext::Compute)
        inCmdBuffer.GetCommandBufferHandle().bindPipeline(vk::PipelineBindPoint::eCompute, mPipeline);
}

} // namespace ksai
