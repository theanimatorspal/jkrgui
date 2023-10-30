#pragma once
#include "VulkanPipelineContext.hpp"
#include "VulkanShaderModule.hpp"
#include "VulkanPipelineLayout.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanPipelineCache.hpp"
#include "VulkanCommandBuffer.hpp"

namespace ksai {

	class VulkanPipelineBase {
	public:
		VulkanPipelineBase(const VulkanDevice& inDevice) : mDevice(inDevice.GetDeviceHandle()) {}
		~VulkanPipelineBase() { mDevice.destroyPipeline(mPipeline); }
		void FillVertexInputDescriptions(const spirv_cross::ShaderResources& Resources, const spirv_cross::Compiler& comp, std::vector<vk::VertexInputBindingDescription>& VertexInputBindingDesp, std::vector<vk::VertexInputAttributeDescription>& InputAttrDescription);;
		template<PipelineContext inContext>
		constexpr void Bind(const VulkanCommandBuffer& inCmdBuffer);
		inline void DrawIndexed(
			const VulkanCommandBuffer& inCmdBuffer,
			int32_t inIndexCount,
			int32_t inInstanceCount,
			int32_t inFirstIndex,
			int32_t inVertexOffset,
			int32_t inFirstInstance
		) const
		{
			inCmdBuffer.GetCommandBufferHandle().drawIndexed(inIndexCount, inInstanceCount, inFirstIndex, inVertexOffset, inFirstIndex);
		}
	public:
		GETTER& GetPipelineHandle() const { return mPipeline; }
	protected:
		const vk::Device& mDevice;
		vk::Pipeline mPipeline;
	};
}


namespace ksai {
	template<size_t NoOfShaderModules, PipelineContext inPipelineContext>
	class VulkanPipeline  : public VulkanPipelineBase
	{
	public:
		VulkanPipeline(const VulkanDevice& inDevice, VulkanPipelineCache &inCache, const VulkanPipelineContextBase& inContext, const VulkanRenderPassBase& inRenderPass, const VulkanPipelineLayoutBase& inLayout, const std::array<VulkanShaderModule, NoOfShaderModules> &inModules);
		~VulkanPipeline() = default;
	};

}


namespace ksai {
	template<PipelineContext inContext>
	inline constexpr void VulkanPipelineBase::Bind(const VulkanCommandBuffer& inCmdBuffer) {
		if constexpr (inContext == PipelineContext::Default) inCmdBuffer.GetCommandBufferHandle().bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);
		else if constexpr (inContext == PipelineContext::Compute) inCmdBuffer.GetCommandBufferHandle().bindPipeline(vk::PipelineBindPoint::eCompute, mPipeline);
	}
}
