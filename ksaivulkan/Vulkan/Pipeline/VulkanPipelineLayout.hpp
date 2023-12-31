#pragma once
#include "VulkanDescriptorSetLayout.hpp"

namespace ksai {
	enum class PipelineContext
	{
		Default,
		DefaultWithGeometry,
		Compute
	};
}


namespace ksai
{
	class VulkanPipelineLayoutBase
	{
	public:
		VulkanPipelineLayoutBase(const VulkanDevice& inDevice) : mDevice(inDevice.GetDeviceHandle()) {}
		~VulkanPipelineLayoutBase() { mDevice.destroyPipelineLayout(mPipelineLayout); }

		void FillPushConstantRanges(const spirv_cross::ShaderResources& VertexResources, const spirv_cross::Compiler& inVertexComp, const spirv_cross::ShaderResources& inFragmentResources, const spirv_cross::Compiler& inFragmentComp, std::vector<vk::PushConstantRange>& PushConstantRanges);
	public:
		GETTER& GetPipelineLayoutHandle() const { return mPipelineLayout; }
	protected:
		const vk::Device& mDevice;
		vk::PipelineLayout mPipelineLayout;
	};
}

namespace ksai {
	template<size_t NoOfShaderModules>
	class VulkanPipelineLayout : public VulkanPipelineLayoutBase
	{
	public:
		VulkanPipelineLayout(const VulkanDevice& inDevice, const VulkanDescriptorSetLayoutBase& inDescriptorSetLayout, const std::array<VulkanShaderModule, NoOfShaderModules>& inModules);
		~VulkanPipelineLayout() = default;
	private:
		const std::array<VulkanShaderModule, NoOfShaderModules>& mModules;
	};
}
