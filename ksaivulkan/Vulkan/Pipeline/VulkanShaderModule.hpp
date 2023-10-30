#pragma once
#include <spirv_cross/spirv_cross.hpp>
#include "VulkanDevice.hpp"

namespace ksai {
	enum class ShaderModuleContext
	{
		Vertex,
		Fragment,
		Geometry,
		Compute
	};

	class VulkanShaderModule
	{
	public:
		VulkanShaderModule(const VulkanDevice& inDevice, const std::vector<uint32_t> &inSPIRV);
		~VulkanShaderModule();
		GETTER& GetShaderModuleHandle() const { return mModule; }
		GETTER& GetShaderResourcesHandle() const { return mResources; }
		GETTER& GetShaderResourcesCompilerHandle() const { return mCompiler; }
	private:
		const vk::Device& mDevice;
		vk::ShaderModule mModule;
		spirv_cross::Compiler mCompiler;
		spirv_cross::ShaderResources mResources;
	};
}
