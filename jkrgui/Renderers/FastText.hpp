#pragma once
#include "Painter.hpp"
#include "AllShaders.hpp"
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "FastText_base.hpp"
#include "Renderer_base.hpp"

namespace Jkr::Renderer
{
	class FastText : public FastText_base, Renderer_base
	{
		using fb = FastText_base;
		using rb = Renderer_base;
	public:
		FastText(const Instance& inInstance, const Window& inCompatibleWindow, const PainterCache& inCache, std::string inFolderPath);
		fb::TextDimensions AddText(const std::string_view inText, uint32_t inX, uint32_t inY, uint32_t& outId);
		fb::TextDimensions UpdateText(const std::string_view inText, uint32_t inX, uint32_t inY);
		void Dispatch(Window& inWindow);
		void DrawInit(Window& inWindow);
		void DrawBatched(Window& inWindow, glm::vec4 inColor, uint32_t inWindowW, uint32_t inWindowH, uint32_t inStartLineId, uint32_t inNoOfLines, glm::mat4 inMatrix = glm::mat4());
	private:
		void CheckAndResize(size_t inNewSize);
		struct PushConstant
		{
			glm::mat4 mMatrix;
			glm::vec4 mColor;
		};
		uint32_t mTotalNoOfCharsRendererCanHold = rb::InitialRendererElementArraySize;
	};
};