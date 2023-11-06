#pragma once
#include "Component_base.hpp"
#include <Renderers/TwoD/2d.hpp>
#include "Component_base.hpp"
#include "EventManager.hpp"
#include "ConfigProps.hpp"

namespace Jkr::Component {
	using namespace Jkr::Renderer;
	class Button : public Component_base
	{
		using cb = Component_base;
	public:
		Button(_2d& inRenderer2D, EventManager& inManager) : r(inRenderer2D), e(inManager) {}
		void BeginDraw();
		void EndDraw();
		void LoadShape();
		void DrawShape(Window& inWindow, uint32_t inW, uint32_t inH, glm::mat4& inMatrix);
		void Event();
	public:
		bool GetButtonShapeId() { return mButonShapeId; }
		bool IsPushed() { return mButtonPushed; }
		bool IsHovered() { return mButtonHovered; }

		SETTER SetButtonNormalColor (glm::vec4 inColor) { mNormalColor = inColor; }
		SETTER SetButtonHoveredColor (glm::vec4 inColor) { mButtonHoveredColor = inColor; }
		SETTER SetButtonClickColor (glm::vec4 inColor) { mClickColor = inColor; }
	protected:
		_2d& r;
		EventManager& e;
	private:
		glm::vec3 mCurrentColor = BUTTON_NORMAL;
		glm::vec3 mButtonHoveredColor = BUTTON_HOVERED;
		glm::vec3 mNormalColor = BUTTON_NORMAL;
		glm::vec3 mClickColor = BUTTON_CLICK;
		uint32_t mButonShapeId;
		uint32_t mBoundRectId;
		bool mButtonPushed = false;
		bool mButtonHovered = false;
		glm::mat4 mTmatrix = glm::identity<glm::mat4>();
	};
}
