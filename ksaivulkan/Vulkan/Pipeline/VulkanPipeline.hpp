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
        VulkanDevice *inDevice;
    };

    VulkanPipelineBase() = default;
    ~VulkanPipelineBase();
    VulkanPipelineBase(VulkanPipelineBase &other)             = delete;
    VulkanPipelineBase &operator=(VulkanPipelineBase &other)  = delete;
    VulkanPipelineBase(VulkanPipelineBase &&other)            = default;
    VulkanPipelineBase &operator=(VulkanPipelineBase &&other) = default;
    operator vk::Pipeline() const { return mPipeline; }

    void Init(CreateInfo inCreateInfo);
    void Destroy();
    GETTER &GetPipelineHandle() const { return mPipeline; }

    void BuildPipeline(VulkanPipelineCache &inCache,
                       VulkanPipelineContextBase &inContext,
                       VulkanRenderPassBase &inRenderPass,
                       VulkanPipelineLayoutBase &inLayout,
                       const std::vector<VulkanShaderModule> &inModules,
                       PipelineContext inPipelineContext,
                       ui inSubpass = 0);

    template <PipelineContext inContext> void Bind(VulkanCommandBuffer &inCmdBuffer) const;
    void DrawIndexed(VulkanCommandBuffer &inCmdBuffer,
                     int32_t inIndexCount,
                     int32_t inInstanceCount,
                     int32_t inFirstIndex,
                     int32_t inVertexOffset,
                     int32_t inFirstInstance) const;
    void FillVertexInputDescriptions(
         const spirv_cross::ShaderResources &Resources,
         const spirv_cross::Compiler &comp,
         std::vector<vk::VertexInputBindingDescription> &VertexInputBindingDesp,
         std::vector<vk::VertexInputAttributeDescription> &InputAttrDescription);

    VulkanPipelineBase(VulkanDevice &inDevice);

    protected:
    vk::Device *mDevice;
    vk::Pipeline mPipeline;
    bool mInitialized = false;
};

// TODO This class is only for backwards compatibility
template <size_t NoOfShaderModules, PipelineContext inPipelineContext>
class VulkanPipeline : public VulkanPipelineBase {
    public:
    struct CreateInfo {};

    VulkanPipeline()                                  = default;
    ~VulkanPipeline()                                 = default;
    VulkanPipeline(VulkanPipeline &other)             = delete;
    VulkanPipeline &operator=(VulkanPipeline &other)  = delete;
    VulkanPipeline(VulkanPipeline &&other)            = default;
    VulkanPipeline &operator=(VulkanPipeline &&other) = default;
    operator vk::Pipeline() const { return mPipeline; }

    void Init(CreateInfo inCreateInfo);
    void Destroy();

    VulkanPipeline(VulkanDevice &inDevice,
                   VulkanPipelineCache &inCache,
                   VulkanPipelineContextBase &inContext,
                   VulkanRenderPassBase &inRenderPass,
                   VulkanPipelineLayoutBase &inLayout,
                   const std::vector<VulkanShaderModule> &inModules);
};

template <PipelineContext inContext>
inline void VulkanPipelineBase::Bind(VulkanCommandBuffer &inCmdBuffer) const {
    if constexpr (inContext == PipelineContext::Default)
        inCmdBuffer.GetCommandBufferHandle().bindPipeline(vk::PipelineBindPoint::eGraphics,
                                                          mPipeline);
    else if constexpr (inContext == PipelineContext::Compute)
        inCmdBuffer.GetCommandBufferHandle().bindPipeline(vk::PipelineBindPoint::eCompute,
                                                          mPipeline);
}

} // namespace ksai
