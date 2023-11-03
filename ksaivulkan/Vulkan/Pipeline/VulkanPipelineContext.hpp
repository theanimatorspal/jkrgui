#pragma once
#include "VulkanDevice.hpp"

namespace ksai {

	enum class PipelinePropertiesContext
	{
		Default,
		Line,
		LineStrip
	};

	class VulkanPipelineContextBase
	{
	public:
		inline constexpr const auto GetTopology() const { return mTopology; }
		inline constexpr const auto GetPolygonMode() const { return mPolygonMode; }
		inline constexpr const auto GetCullMode() const { return mCullMode; }
		inline constexpr const auto GetFrontFace() const { return mFrontFace; }
		inline constexpr const auto& GetDynamicStates() const { return mDynamicStates; }
	protected:
		vk::PrimitiveTopology mTopology = vk::PrimitiveTopology::eTriangleList;
		vk::PolygonMode mPolygonMode = vk::PolygonMode::eFill;
		vk::CullModeFlags mCullMode = vk::CullModeFlagBits::eFront;
		vk::FrontFace mFrontFace = vk::FrontFace::eCounterClockwise;
		std::array<vk::DynamicState, 2> mDynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
	};
}

namespace ksai {
	class VulkanPipelineContext : public VulkanPipelineContextBase
	{
	public:
		constexpr VulkanPipelineContext(PipelinePropertiesContext inContext)
		{
			if (inContext == PipelinePropertiesContext::Default)
			{
				mTopology = vk::PrimitiveTopology::eTriangleList;
				mPolygonMode = vk::PolygonMode::eFill;
				mCullMode = vk::CullModeFlagBits::eFront;
				mFrontFace = vk::FrontFace::eCounterClockwise;
			}
			if (inContext == PipelinePropertiesContext::Line)
			{
				mTopology = vk::PrimitiveTopology::eLineList;
				mPolygonMode = vk::PolygonMode::eLine;
				mCullMode = vk::CullModeFlagBits::eNone;
				mFrontFace = vk::FrontFace::eCounterClockwise;
			}
			if (inContext == PipelinePropertiesContext::LineStrip)
			{
				mTopology = vk::PrimitiveTopology::eLineStrip;
				mPolygonMode = vk::PolygonMode::eLine;
				mCullMode = vk::CullModeFlagBits::eNone;
				mFrontFace = vk::FrontFace::eCounterClockwise;

			}
		}
	};
}
