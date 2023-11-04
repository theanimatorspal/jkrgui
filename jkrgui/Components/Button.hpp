#pragma once
#include "Component_base.hpp"
#include <Renderers/TwoD/2d.hpp>
#include "Component_base.hpp"
#include "EventManager.hpp"
#include "ConfigProps.hpp"

namespace Jkr::Component {
	using namespace Jkr::Renderer;
	class Button
	{
	public:
		Button(_2d& inRenderer2D, EventManager& inManager) : r(inRenderer2D), e(inManager) {}
		void LoadShape();
		void DrawShape(Window& inWindow, uint32_t inW, uint32_t inH, glm::mat4& inMatrix);
		void Event();
	public:
		bool GetButtonShapeId() { return mButonShapeId; }
		bool IsPushed() { return mButtonPushed; }
		bool IsHovered() { return mButtonHovered; }

		SETTER SetPos (glm::uvec2 inPos) { mPos = inPos; }
		SETTER SetDimensions (glm::uvec2 inDimen) { mDimen = inDimen; }
		SETTER SetDepthLevel (uint32_t inDepthLevel) { mDepthLevel = inDepthLevel; }

		GETTER GetPos() { return mPos; }
		GETTER GetDimensions() { return mDimen; }
		GETTER GetDepthLevel() { return mDepthLevel; }
	protected:
		_2d& r;
		EventManager& e;
	private:
		uint32_t mDepthLevel;
		glm::uvec2 mDimen;
		glm::uvec2 mPos;
		glm::vec3 mCurrentColor = BUTTON_NORMAL;
		uint32_t mButonShapeId;
		uint32_t mBoundRectId;
		bool mButtonPushed = false;
		bool mButtonHovered = false;
	};
}
