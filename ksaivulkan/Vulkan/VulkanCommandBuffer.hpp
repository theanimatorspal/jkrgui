#pragma once
#include "VulkanCommandPool.hpp"
#include "VulkanDevice.hpp"
#include "VulkanFrameBuffer.hpp"
#include <Pipeline/VulkanPipelineLayout.hpp>
namespace ksai {

class VulkanCommandBuffer {
public:
    enum class Type {
        Primary,
        Secondary
    };
    enum class BeginContext {
		Normal,
		ContinueRenderPass
    };

    VulkanCommandBuffer(const VulkanDevice& inDevice, const VulkanCommandPool& inPool, Type inContext = Type::Primary);
    ~VulkanCommandBuffer();
    GETTER& GetCommandBufferHandle() const { return mBuffer; }
    template<BeginContext inContext = BeginContext::Normal>
    void Begin() const;
    void Reset() const { mBuffer.reset(); }
    void End() const { mBuffer.end(); }

    enum class RenderPassBeginContext {
		Inline,
		SecondaryCommandBuffers
    };
    template<RenderPassBeginContext inBeginContext = RenderPassBeginContext::Inline>
    void BeginRenderPass(const VulkanRenderPassBase& inRenderPass, const VulkanSurface& inSurface, const VulkanFrameBufferBase& inFrameBuffer, std::array<float, 5> inClearValue) const;
    void EndRenderPass() const;

    template <class T>
    void PushConstants(const VulkanPipelineLayoutBase& inPipelineLayout, const vk::ArrayProxy<const T> inValues) const
    {
        mBuffer.pushConstants<T>(inPipelineLayout.GetPipelineLayoutHandle(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eCompute, 0, inValues);
    }

private:
    const vk::Device& mDevice;
    vk::CommandBuffer mBuffer;
};

}
