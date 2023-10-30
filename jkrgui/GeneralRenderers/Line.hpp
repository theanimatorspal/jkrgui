#pragma once
#include "Line_base.hpp"
#include "Renderer_base.hpp"

namespace Jkr::Renderer {
	class Line : public Line_base, Renderer_base
	{
		using rb = Renderer_base;
		using lb = Line_base;
	public:
		Line(const Instance& inInstance, GUIWindow& inCompatibleWindow, GUIPainterCache& inPainterCache);
		void AddLine(glm::vec2 inFirstPoint, glm::vec2 inSecondPoint, float inDepthValue, uint32_t& outId);
		void UpdateLine(uint32_t inId, glm::vec2 inFirstPoint, glm::vec2 inSecondPoint, float inDepthValue);
		void Dispatch(GUIWindow& inWindow);
		void DrawInit(GUIWindow& inWindow);
		void DrawBatched(GUIWindow& inWindow, glm::vec4 inColor, uint32_t inWindowW, uint32_t inWindowH, uint32_t inLineOffset, uint32_t inNoOfLines);
		void DrawSingle(GUIWindow& inWindow, glm::vec4 inColor, uint32_t inWindowW, uint32_t inWindowH, uint32_t inId);
	private:
		struct PushConstant {
			glm::mat4 mMatrix;
			glm::vec4 mColor;
		};
		uint32_t mTotalNoOfLinesRendererCanHold = 10;
		Up<GUIPrimitive> mPrimitive;
		Up<GUIPainter> mPainter;
	private:
		void CheckAndResize(const Instance& inInstance, uint32_t inNewSizeNeeded);
		const Instance& mInstance;
	};
}
