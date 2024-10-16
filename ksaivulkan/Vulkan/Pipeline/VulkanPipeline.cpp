#include "VulkanPipeline.hpp"
#include "Pipeline/VulkanShaderModule.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_enums.hpp"

namespace ksai {
void VulkanPipelineBase::FillVertexInputDescriptions(
     const spirv_cross::ShaderResources &Resources,
     const spirv_cross::Compiler &comp,
     std::vector<vk::VertexInputBindingDescription> &VertexInputBindingDesp,
     std::vector<vk::VertexInputAttributeDescription> &InputAttrDescription) {
    {
        // uint32_t previousBindingIndex = 0;
        uint32_t AttributeOffset = 0;
        InputAttrDescription.resize(Resources.stage_inputs.size());
        v<ui> AttrSizes(InputAttrDescription.size());
        for (auto i = 0; i < Resources.stage_inputs.size(); i++) {
            auto &u                    = Resources.stage_inputs[i];
            spirv_cross::SPIRType type = comp.get_type(u.type_id);
            size_t AttributeSize       = 0; // TODO
            uint32_t location          = comp.get_decoration(u.id, spv::DecorationLocation);
            uint32_t bindingIndex      = comp.get_decoration(u.id, spv::DecorationBinding);
            vk::Format format          = vk::Format::eUndefined; // TODO

            uint32_t siz               = type.vecsize;
            switch (siz) {
                case 1:
                    if (type.basetype == spirv_cross::SPIRType::Float)
                        format = vk::Format::eR32Sfloat;
                    if (type.basetype == spirv_cross::SPIRType::Int) format = vk::Format::eR32Sint;
                    AttributeSize = static_cast<size_t>(1) * static_cast<uint32_t>(sizeof(float));
                    break;
                case 2:
                    if (type.basetype == spirv_cross::SPIRType::Float)
                        format = vk::Format::eR32G32Sfloat;
                    if (type.basetype == spirv_cross::SPIRType::Int)
                        format = vk::Format::eR32G32Sint;

                    /// @warning std430 alignment rule is followed here
                    /// IDK WTF, align EVERYTHING as 16 fucking bytes
                    AttributeSize = static_cast<size_t>(4) * static_cast<uint32_t>(sizeof(float));
                    break;
                case 3:
                    if (type.basetype == spirv_cross::SPIRType::Int)
                        format = vk::Format::eR32G32B32Sint;
                    if (type.basetype == spirv_cross::SPIRType::Float)
                        format = vk::Format::eR32G32B32Sfloat;
                    /// @warning std430 alignment rule is followed here
                    AttributeSize = static_cast<size_t>(4) * static_cast<uint32_t>(sizeof(float));
                    break;
                case 4:
                    if (type.basetype == spirv_cross::SPIRType::Int)
                        format = vk::Format::eR32G32B32A32Sint;
                    if (type.basetype == spirv_cross::SPIRType::Float)
                        format = vk::Format::eR32G32B32A32Sfloat;
                    AttributeSize = static_cast<size_t>(4) * static_cast<uint32_t>(sizeof(float));
                    break;
            }
            AttrSizes[location]     = AttributeSize;
            InputAttrDescription[i] = vk::VertexInputAttributeDescription(
                 location, bindingIndex, format, AttributeOffset);
            AttributeOffset += AttributeSize;
        }
        std::sort(InputAttrDescription.begin(),
                  InputAttrDescription.end(),
                  [](const auto &lhs, const auto &rhs) { return lhs.location < rhs.location; });

        for (ui off = 0, i = 0; auto &u : InputAttrDescription) {
            u.offset = off;
            off += AttrSizes[i];
            i++;
        }

        const auto InputBindingWholeSize = AttributeOffset;
        const auto FirstBindingIndex     = 0;
        if (Resources.stage_inputs.size() > 0) {
            VertexInputBindingDesp.push_back(
                 vk::VertexInputBindingDescription(FirstBindingIndex, InputBindingWholeSize));
        }
    }
}

} // namespace ksai

namespace ksai {
template <>
VulkanPipeline<2, PipelineContext::Default>::VulkanPipeline(
     VulkanDevice &inDevice,
     VulkanPipelineCache &inCache,
     VulkanPipelineContextBase &inContext,
     VulkanRenderPassBase &inRenderPass,
     VulkanPipelineLayoutBase &inLayout,
     const std::vector<VulkanShaderModule> &inModules)
    : VulkanPipelineBase(inDevice) {
    BuildPipeline(inCache, inContext, inRenderPass, inLayout, inModules, PipelineContext::Default);
}

} // namespace ksai

namespace ksai {
template <>
VulkanPipeline<1, PipelineContext::Compute>::VulkanPipeline(
     VulkanDevice &inDevice,
     VulkanPipelineCache &inCache,
     VulkanPipelineContextBase &inContext,
     VulkanRenderPassBase &inRenderPass,
     VulkanPipelineLayoutBase &inLayout,
     const std::vector<VulkanShaderModule> &inModules)
    : VulkanPipelineBase(inDevice) {
    BuildPipeline(inCache, inContext, inRenderPass, inLayout, inModules, PipelineContext::Compute);
}

void VulkanPipelineBase::BuildPipeline(VulkanPipelineCache &inCache,
                                       VulkanPipelineContextBase &inContext,
                                       VulkanRenderPassBase &inRenderPass,
                                       VulkanPipelineLayoutBase &inLayout,
                                       const std::vector<VulkanShaderModule> &inModules,
                                       PipelineContext inPipelineContext,
                                       ui inSubpass) {
    if (inModules.size() != 0) {
        if (inPipelineContext == PipelineContext::Compute) {
            auto ShaderStageCreateInfo =
                 vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(),
                                                   vk::ShaderStageFlagBits::eCompute,
                                                   inModules[0].GetShaderModuleHandle(),
                                                   "Glsl"
                                                   "Mai"
                                                   "n");
            auto ComputePipelineCreateInfo =
                 vk::ComputePipelineCreateInfo(vk::PipelineCreateFlags(),
                                               ShaderStageCreateInfo,
                                               inLayout.GetPipelineLayoutHandle());
            auto Result = mDevice->createComputePipeline(inCache.GetPipelineCacheHandle(),
                                                         ComputePipelineCreateInfo);
            mPipeline   = Result.value;
        } else {
            auto &VertexShaderCompiler  = inModules[0].GetShaderResourcesCompilerHandle();
            auto &VertexShaderResources = inModules[0].GetShaderResourcesHandle();
            std::vector<vk::VertexInputBindingDescription> VertexInputBindingDesp;

            std::vector<vk::VertexInputAttributeDescription> InputAttrDescription;

            FillVertexInputDescriptions(VertexShaderResources,
                                        VertexShaderCompiler,
                                        VertexInputBindingDesp,
                                        InputAttrDescription);
            auto PipelineVertexInputStateCreateInfo =
                 vk::PipelineVertexInputStateCreateInfo(vk::PipelineVertexInputStateCreateFlags(),
                                                        VertexInputBindingDesp,
                                                        InputAttrDescription);
            auto PipelineInputAssemblyStateCreateInfo = vk::PipelineInputAssemblyStateCreateInfo(
                 vk::PipelineInputAssemblyStateCreateFlags(), inContext.GetTopology());
            auto PipelineTessellationStateCreateInfo = vk::PipelineTessellationStateCreateInfo(
                 vk::PipelineTessellationStateCreateFlagBits());
            auto PipelineViewportStateCreateInfo = vk::PipelineViewportStateCreateInfo(
                 vk::PipelineViewportStateCreateFlags(), 1, nullptr, 1, nullptr);
            auto PipelineRasterizationStateCreateInfo = vk::PipelineRasterizationStateCreateInfo(
                 vk::PipelineRasterizationStateCreateFlags(),
                 false,
                 false,
                 inContext.GetPolygonMode(),
                 inContext.GetCullMode(),
                 inContext.GetFrontFace(),
                 false,
                 0.0f,
                 0.0f,
                 0.0f,
                 1.0f);
            auto PipelineMultisampleStateCreateInfo =
                 vk::PipelineMultisampleStateCreateInfo(vk::PipelineMultisampleStateCreateFlags(),
                                                        // TODO
                                                        // Add
                                                        // this
                                                        // to
                                                        // Parameter
                                                        vk::SampleCountFlagBits::e4);
            auto StencilOpState = vk::StencilOpState(vk::StencilOp::eKeep,
                                                     vk::StencilOp::eKeep,
                                                     vk::StencilOp::eKeep,
                                                     vk::CompareOp::eAlways);
            auto PipelineDepthStencilStateCreateInfo =
                 vk::PipelineDepthStencilStateCreateInfo(vk::PipelineDepthStencilStateCreateFlags(),
                                                         true,
                                                         true,
                                                         vk::CompareOp::eLessOrEqual,
                                                         false,
                                                         false,
                                                         StencilOpState,
                                                         StencilOpState);
            auto ColorComponentFlags =
                 vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                 vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
            auto PipelineColorBlendAttachmentState =
                 vk::PipelineColorBlendAttachmentState(true,
                                                       vk::BlendFactor::eSrcAlpha,
                                                       vk::BlendFactor::eOneMinusSrcAlpha,
                                                       vk::BlendOp::eAdd,
                                                       vk::BlendFactor::eSrcAlpha,
                                                       vk::BlendFactor::eOneMinusSrcAlpha,
                                                       vk::BlendOp::eAdd,
                                                       ColorComponentFlags);
            std::vector<vk::PipelineColorBlendAttachmentState> PipelineColorBlendAttachmentStates;
            PipelineColorBlendAttachmentStates.resize(inRenderPass.GetAttachmentCount(),
                                                      PipelineColorBlendAttachmentState);
            auto PipelineColorBlendStateCreateInfo =
                 vk::PipelineColorBlendStateCreateInfo(vk::PipelineColorBlendStateCreateFlags(),
                                                       false,
                                                       vk::LogicOp::eCopy,
                                                       PipelineColorBlendAttachmentStates,
                                                       {{1.0f, 1.0f, 1.0f, 1.0f}});

            std::vector<vk::PipelineShaderStageCreateInfo> PipelineShaderStageCreateInfos = {
                 vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(),
                                                   vk::ShaderStageFlagBits::eVertex,
                                                   inModules[0].GetShaderModuleHandle(),
                                                   "GlslMain"),
                 vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(),
                                                   vk::ShaderStageFlagBits::eFragment,
                                                   inModules[1].GetShaderModuleHandle(),
                                                   "GlslMain"),
            };

            auto PipelineDynamicStateCreateInfo = vk::PipelineDynamicStateCreateInfo(
                 vk::PipelineDynamicStateCreateFlags(), inContext.GetDynamicStates());

            if (inPipelineContext == PipelineContext::Skybox) {
                PipelineColorBlendAttachmentState.setBlendEnable(false);
                PipelineRasterizationStateCreateInfo.setCullMode(vk::CullModeFlagBits::eNone)
                     .setFrontFace(vk::FrontFace::eCounterClockwise);
            }

            if (inPipelineContext == PipelineContext::Shadow) {
                PipelineShaderStageCreateInfos.resize(1); // Vertex Shader Only
                PipelineMultisampleStateCreateInfo.setRasterizationSamples(
                     vk::SampleCountFlagBits::e1);
            }

            if (inPipelineContext == PipelineContext::DefaultSingleSampled) {
                PipelineMultisampleStateCreateInfo.setRasterizationSamples(
                     vk::SampleCountFlagBits::e1);
            }

            auto GraphicsPipelineCreateInfo =
                 vk::GraphicsPipelineCreateInfo(vk::PipelineCreateFlags(),
                                                PipelineShaderStageCreateInfos,
                                                &PipelineVertexInputStateCreateInfo,
                                                &PipelineInputAssemblyStateCreateInfo,
                                                &PipelineTessellationStateCreateInfo,
                                                &PipelineViewportStateCreateInfo,
                                                &PipelineRasterizationStateCreateInfo,
                                                &PipelineMultisampleStateCreateInfo,
                                                &PipelineDepthStencilStateCreateInfo,
                                                &PipelineColorBlendStateCreateInfo,
                                                &PipelineDynamicStateCreateInfo);

            GraphicsPipelineCreateInfo.setSubpass(inSubpass);
            GraphicsPipelineCreateInfo.setRenderPass(inRenderPass.GetRenderPassHandle())
                 .setLayout(inLayout.GetPipelineLayoutHandle());
            mPipeline = mDevice->createGraphicsPipeline(nullptr, GraphicsPipelineCreateInfo).value;
        }
    }
}

void VulkanPipelineBase::DrawIndexed(VulkanCommandBuffer &inCmdBuffer,
                                     int32_t inIndexCount,
                                     int32_t inInstanceCount,
                                     int32_t inFirstIndex,
                                     int32_t inVertexOffset,
                                     int32_t inFirstInstance) const {
    inCmdBuffer.GetCommandBufferHandle().drawIndexed(
         inIndexCount, inInstanceCount, inFirstIndex, inVertexOffset, inFirstIndex);
}

VulkanPipelineBase::~VulkanPipelineBase() {
    if (mInitialized) {
        Destroy();
    }
}

VulkanPipelineBase::VulkanPipelineBase(VulkanDevice &inDevice) { Init({&inDevice}); }

void VulkanPipelineBase::Init(CreateInfo info) {
    mDevice      = &info.inDevice->GetDeviceHandle();
    mInitialized = true;
}

void VulkanPipelineBase::Destroy() {
    mDevice->waitIdle();
    mDevice->destroyPipeline(mPipeline);
    mInitialized = false;
}

} // namespace ksai
