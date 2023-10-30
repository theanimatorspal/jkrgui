#pragma once
#include "Font/FastText.hpp"
#include "GUIPainter.hpp"
#include "AllShaders.hpp"
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Jkr::Font
{
	class FastTextRenderer
	{
	private:
		using AtlasType = Jkr::GUIPainterParameter<Jkr::GUIPainterParameterContext::UniformImage>;
		using StagingBuffer = Jkr::VulkanBufferVMA<BufferContext::Staging, MemoryType::HostVisibleAndHostCached>;
		struct CharMemoryRange {
			kstd::ui32 mStart;
			kstd::ui32 mSize;
		};
	public:
		enum class HorizontalAlignment { Left, Right, Center };
		enum class VerticalAlignment { Top, Bottom, Center };
		struct FeatureFlags { HorizontalAlignment h = HorizontalAlignment::Left; VerticalAlignment v = VerticalAlignment::Bottom; kstd::f32 inSizeFactor = 1.0f; };
	public:
		FastTextRenderer(const Instance& inInstance, const GUIWindow& inWindow, const GUIPainterCache& inCache, std::string inFolderPath);
		void Dispatch();
		void CheckResize(int ResizeFactor);
		void RegisterStaticText(const std::string inString, kstd::ui32 inX, kstd::ui32 inY, FeatureFlags inFlags = FeatureFlags{ .h = HorizontalAlignment::Left, .v = VerticalAlignment::Bottom, .inSizeFactor = 1 }, kstd::ui32 inDepthValue = 0);
		void RegisterDynamicText(kstd::ui32 inCharCount, kstd::ui32& outId);
		void UpdateDynamicText(kstd::ui32 inId, const char* inString, kstd::ui32 inX, kstd::ui32 inY, FeatureFlags inFlags = FeatureFlags{ .h = HorizontalAlignment::Left, .v = VerticalAlignment::Bottom, .inSizeFactor = 1 }, kstd::ui32 inDepthValue = 0);
		GETTER GetTextFeatures(const std::string inString, float inSizeFactor = 1.0f) const { return mFastTexts->GetTextFeatures(inString.c_str(), 1/inSizeFactor); }
		inline void Draw(kstd::ui32 WindowWidth, kstd::ui32 WindowHeight, const GUIWindow& inWindow)
		{
			glm::mat4 Matrix = glm::ortho(
				0.0f,
				static_cast<float>(WindowWidth),
				0.0f,
				static_cast<float>(WindowHeight),
				100.0f,
				-100.0f
			);
			mPainter->Draw<glm::mat4>(*mPrimitive, Matrix, inWindow);
		}
		inline void Draw(kstd::ui32 WindowWidth, kstd::ui32 WindowHeight)
		{
			Draw(WindowWidth, WindowHeight, mWindow);
		}
	private:
		kstd::ui32 mNoOfCharactersDrawn = 0;
		kstd::ui32 mNoOfCharactersStaged = 0;
		std::vector<kstd::Vertex> mT_Vertices;
		std::vector<kstd::ui32> mT_Indices;
		Up<ksai::Font::FastText> mFastTexts;
		std::vector<CharMemoryRange> mDynamicTextCharacterIndexRanges;
	private:
		const Instance& mInstance;
		const GUIWindow& mWindow;
		Up<AtlasType> mAtlases;
		Up<GUIPainter> mPainter;
		Up<Jkr::GUIPrimitive> mPrimitive;
	private:
		Up<StagingBuffer> mStagingVertexBuffer;
		Up<StagingBuffer> mStagingIndexBuffer;
		void* mVertexStagingBufferMemoryMapPtr = nullptr;
		void* mIndexStagingBufferMemoryMapPtr = nullptr;
		kstd::ui32 mNoOfCharsToCopyInStagingBuffer = 0;
		std::vector<vk::BufferCopy> mVertexCopyRegions, mIndexCopyRegions;
		void CopyToMainPrimitiveFromStagingBuffer(kstd::ui32 InFromBufferOffsetIndex, kstd::ui32 InToBufferOffsetIndex, kstd::ui32 inSizeToCopyIndex);
	private:
		uint32_t mTotalNoOfCharsStagingBufferCanHold = 10;
		uint32_t mTotalNoOfCharsRendererCanHoldAtOnce = 10;
		const size_t sizeOfVertices = sizeof(kstd::Vertex) * 4;
		const size_t sizeOfIndices = sizeof(uint32_t) * 6;
	};
}


