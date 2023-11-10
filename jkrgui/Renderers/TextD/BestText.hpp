
#pragma once
#include "Painter.hpp"
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "BestText_base.hpp"
#include "../Renderer_base.hpp"

namespace Jkr::Renderer
{
	class BestText : public BestText_base, Renderer_base
	{
		using bb = BestText_base;
		using rb = Renderer_base;
	public:
		BestText ( const Instance& inInstance, const Window& inCompatibleWindow, const PainterCache& inCache );
		bb::TextDimensions AddText ( const std::string_view inText, uint32_t inX, uint32_t inY, uint32_t inDepthValue, uint32_t& outId );
		bb::TextDimensions UpdateText ( uint32_t inId, const std::string_view inText, uint32_t inX, uint32_t inY, uint32_t inDepthValue );
		void SetCurrentFontFace ( uint32_t inFontFaceId );
		void Dispatch ( Window& inWindow );
		void Bind ( Window& inWindow );
		void Draw ( Window& inWindow, glm::vec4 inColor, uint32_t inWindowW, uint32_t inWindowH, uint32_t inStartTextId, uint32_t inNoOfChars, glm::mat4 inMatrix );
		void CheckAndResize ( size_t inNewSize );
	private:
		uint32_t mCurrentFontFaceId = 0;
	private:
		const Instance& mInstance;
		struct PushConstant
		{
			glm::mat4 mMatrix;
			glm::vec4 mColor;
		};
		uint32_t mTotalNoOfCharsRendererCanHold = rb::InitialRendererElementArraySize;
		std::unordered_map<uint32_t, uint32_t> mCodepointKeyTextureMap;
	private:
		Up<Primitive> mPrimitive;
		Up<Painter> mPainter;
		Up<UniformType> mUniformBuffer;

#ifndef JKR_NO_STAGING_BUFFERS
		Up<StagingBuffer> mStagingBuffer;
#endif

#ifdef JKR_USE_VARIABLE_DES_INDEXING
		std::vector<Up<ImageType>> mGlyphImages;
#else 
		//Yet to implement TODO
#endif
	private:
	};
};
