#pragma once
#include "Painter.hpp"
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "FastText_base.hpp"
#include "../Renderer_base.hpp"

// TODO: Either Remove this or Make it Better

namespace Jkr::Renderer
{
	class FastText : public FastText_base, Renderer_base
	{
		using fb = FastText_base;
		using rb = Renderer_base;
	public:
		FastText(const Instance& inInstance, const Window& inCompatibleWindow, const PainterCache& inCache);
		fb::TextDimensions AddText(const std::string_view inText, uint32_t inX, uint32_t inY, uint32_t inDepthValue, uint32_t& outId);
		fb::TextDimensions UpdateText(uint32_t inId, const std::string_view inText, uint32_t inX, uint32_t inY, uint32_t inDepthValue);
		void Dispatch(Window& inWindow);
		void Bind(Window& inWindow);
		void Draw(Window& inWindow, glm::vec4 inColor, uint32_t inWindowW, uint32_t inWindowH, uint32_t inStartLineId, uint32_t inNoOfChars, glm::mat4 inMatrix);
		void DrawAll(Window& inWindow, glm::vec4 inColor, uint32_t inWindowW, uint32_t inWindowH, glm::mat4 inMatrix)
		{
			Draw(inWindow, inColor, inWindowW, inWindowH, 0, GetCurrentCharOffsetAbsolute(), inMatrix);
		}
	private:
		const Instance& mInstance;
		void CheckAndResize(size_t inNewSize);
		struct PushConstant
		{
			glm::mat4 mMatrix;
			glm::vec4 mColor;
		};
		uint32_t mTotalNoOfCharsRendererCanHold = rb::InitialRendererElementArraySize;
	private:
		Up<Primitive> mPrimitive;
		Up<Painter> mPainter;
		Up<ImageType> mImage;
	private:
	};
};