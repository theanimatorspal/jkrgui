#pragma once
#include "GUIPainter.hpp"
#include "AllShaders.hpp"
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Jkr {
	class GUI2DRenderer
	{
	public:
		/*
			Type Aliases for Different Types
		*/
		using ImageType = Jkr::GUIPainterParameter<Jkr::GUIPainterParameterContext::StorageImage>;
		using StagingBuffer = Jkr::VulkanBufferVMA<BufferContext::Staging, MemoryType::HostVisibleAndHostCached>;
		using SamplerType = Jkr::GUIPainterParameter<Jkr::GUIPainterParameterContext::UniformSampler>;
		using UniformType = Jkr::GUIPainterParameter<Jkr::GUIPainterParameterContext::UniformBuffer>;


	public:
		/*
			The following is the interface of the Renderer to be used
		*/
		enum class Shape : kstd::ui32
		{
			Circle = 0,
			Rectangle = 1,
			Line = 2,
			Fill = 3,
			GridFill = 4
		};
		GUI2DRenderer(const Instance& inInstance, const GUIWindow& inWindow, const GUIPainterCache& inCache);
		GUI2DRenderer(const Instance& inInstance, const GUIWindow& inWindow, const GUIPainterCache& inCache, void* inExtensionForDrawCall);

		void Draw(kstd::ui32 inWindowW, kstd::ui32 inWindowH, const GUIWindow& inWindow);
		void Draw_WithSeparateDrawCallsPerQuad(const glm::mat4& Matrix, const Jkr::GUIWindow& inWindow);
		void Draw(kstd::ui32 inWindowW, kstd::ui32 inWindowH);
		void DrawQuadWithExistingTextureCoordinate(kstd::ui32 inId, glm::vec3 mColor, kstd::ui32 inWindowW, kstd::ui32 inWindowH, const GUIWindow& inWindow);

		void Dispatch(float inTime);
		void RegisterStaticQuad(kstd::ui32 inX, kstd::ui32 inY, kstd::ui32 inW, kstd::ui32 inH, kstd::ui32& outTextureId, const std::function<void(GUI2DRenderer&, kstd::ui32)>& inDrawFunction = [](const GUI2DRenderer&, kstd::ui32) {}, kstd::ui32 inDepthValue = 0, kstd::ui32 inTextureWidth = 100, kstd::ui32 inTextureHeight = 100);
		void RegisterStaticQuadWithExistingTexture(kstd::ui32& outIndex, kstd::ui32 inTextureId, kstd::ui32 inX, kstd::ui32 inY, kstd::ui32 inW, kstd::ui32 inH, kstd::ui32 inDepthValue);
		void RegisterDynamicQuad(kstd::ui32& outId, const std::function<void(GUI2DRenderer&, kstd::ui32)>& inDrawFunction = [](const GUI2DRenderer&, kstd::ui32) {}, kstd::ui32 inTextureWidth = 100, kstd::ui32 inTextureHeight = 100, kstd::ui32 inDepthValue = 10);
		void RegisterDynamicQuadWithExistingTexture(kstd::ui32& outIndex, kstd::ui32 inTextureId, kstd::ui32 inX, kstd::ui32 inY, kstd::ui32 inW, kstd::ui32 inH, kstd::ui32 inDepthValue);
		void UpdateDynamicQuad(kstd::ui32 inId, kstd::ui32 inX, kstd::ui32 inY, kstd::ui32 inW, kstd::ui32 inH, kstd::ui32 inDepthValue = 0);
		void UpdateDynamicQuadWithExistingTexture(kstd::ui32 inId, kstd::ui32 inX, kstd::ui32 inY, kstd::ui32 inW, kstd::ui32 inH, kstd::ui32 inDepthValue = 0);

		void CheckResize(kstd::ui32 inNewCharactersRequired);
		void DrawToQuadTextureDispatch(kstd::ui32 inId, Shape inShape, glm::vec4 mPosDimen, glm::vec4 inColor, glm::vec4 inParam);

	private:
		/*
			The following are the members from Jkr::GUI
		*/
		const Instance& mInstance;
		const GUIWindow& mWindow;
		Up<GUIPainter> mPainter;
		Up<GUIPrimitive> mPrimitive;
		std::vector<Up<ImageType>> mTextures;



	private:
		/*
			These are required for logical working of the renderer
		*/
		struct ComputePushConstantStruct {
			glm::vec4 mPosDimen;
			glm::vec4 mColor;
			glm::vec4 mParam;
			int mTextureCoordinateIndex;
			int mShapeIndex;
		};

		struct FragmentPushConstantStruct
		{
			glm::mat4 mMatrix;
			glm::vec4 mColor;
			int index;
		};

		uint32_t mTotalNoOfQuadsStagingBufferCanHold = 10;
		uint32_t mTotalNoOfQuadsRendererCanHold = 10;
		const size_t sizeOfVertices = sizeof(kstd::VertexEXT) * 4;
		const size_t sizeOfIndices = sizeof(uint32_t) * 6;
		kstd::ui32 mDstBindingForImages = 0;

		std::vector<kstd::VertexEXT> mT_Vertices;
		std::vector<kstd::ui32> mT_Indices;
		kstd::ui32 mNoOfQuadsDrawn = 0;
		kstd::ui32 mNoOfQuadsToCopyInStagingBufferToMainBuffer = 0; // TODO TO main Buffer from Staging Buffer

		std::vector<std::function<void(GUI2DRenderer&, kstd::ui32)>> mDrawFunctions;
		std::vector<kstd::ui32> mTextureCoordinateIndicesOfAllQuads;
		/*
			The Quads With Existing Texture Indices Need to be manually drawn
		*/
		struct QuadsWithTextureIndex { kstd::ui32 mTextureIndex; size_t mQuadIndex; };
		std::vector<QuadsWithTextureIndex> mQuadsWithExistingTextureIndex;
		std::vector<QuadsWithTextureIndex> mQuadsWithItsOwnTextureIndex;

		kstd::ui32 GenerateTheVerticesAndIndices(const ksai::kstd::ui32 inX, const ksai::kstd::ui32 inW, const ksai::kstd::ui32 inY, const ksai::kstd::ui32 inH, unsigned int v_index, unsigned int i_index, kstd::ui32 inDepthValue = 0);
		void GenerateTextureCoordinates(glm::vec2& inFirst, glm::vec2& inSecond, glm::vec2& inThird, glm::vec2& inFourth);
		void CopyTheGeneratedQuadToStagingBuffer(const ksai::kstd::ui32 inOffset, const ksai::kstd::ui32 NoOfQuadsGenerated);

	private:
		/*
			These are lower level Vulkan Handles
		*/
		Up<StagingBuffer> mStagingVertexBuffer;
		Up<StagingBuffer> mStagingIndexBuffer;
		void* mVertexStagingBufferMemoryMapPtr = nullptr;
		void* mIndexStagingBufferMemoryMapPtr = nullptr;
		void CopyToMainPrimitiveFromStagingBuffer(kstd::ui32 InFromBufferOffsetIndex, kstd::ui32 InToBufferOffsetIndex, kstd::ui32 inSizeToCopyIndex);
		std::vector<vk::BufferCopy> mDynamicQuadVertexCopyRegions;
		std::vector<vk::BufferCopy> mDynamicQuadIndexCopyRegions;

		Up<UniformType> mUniformBuffer;


	};
}

