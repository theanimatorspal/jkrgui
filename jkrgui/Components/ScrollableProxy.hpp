#pragma once
#include "Component_base.hpp"
#include <Renderers/TwoD/2d.hpp>
#include "Components/Button.hpp"
#include "EventManager.hpp"


namespace Jkr::Component
{
	using namespace Jkr::Renderer;
	class ScrollableProxy : public Component_base
	{
		using cb = Component_base;
	public:
		ScrollableProxy(_2d& inRenderer2D, EventManager& inManager) : r(inRenderer2D), e(inManager) { }
		void BeginDraw();
		void EndDraw();
		void Load(uint32_t inW, uint32_t inH);
		void Update(uint32_t inW, uint32_t inH);
		void DrawLines(Window& inWindow, uint32_t inW, uint32_t inH);
		void DrawShapes(Window& inWindow, uint32_t inW, uint32_t inH);
		void Dispatch();
		void Event(Window& inWindow, uint32_t inW, uint32_t inH);
	protected:
		_2d& r;
		EventManager& e;
		SETTER SetOffset(glm::vec2 inOffset) { mOffset = inOffset; }
		SETTER SetScrollBarWidth(uint32_t in) { width = in; mScrollBarSize = in; }
		SETTER SetScrollBarHeight(uint32_t in) { height = in; }
		GETTER GetClippingMatrix() { 
			return glm::ortho(
				cb::GetPosition().x,
				cb::GetPosition().x + cb::GetDimension().x,
				cb::GetPosition().y + cb::GetDimension().y,
				cb::GetPosition().y,
				100.0f,
				-100.0f
			); 
		}
	private:
		glm::mat4 mTransform = glm::identity<glm::mat4>();
		glm::vec2 mOffset;
		uint32_t mBackgroundShapeId;
		uint32_t mHorizontalScrollBarBackgroundShapeId;
		uint32_t mVerticalScrollBarBackgroundShapeId;
		uint32_t mScrollBarSize = 10;
		Up<Button> mHorizontalButton;
		Up<Button> mVerticalButton;
		glm::vec4 mBackgroundColor = glm::vec4(color_BLACK, 1.0);
		glm::vec4 mScrollBarBackgroundColor = glm::vec4(color_GRAY / 4.0f, 1.0);
	private:
		bool ScrollBarBeingScrolled = false;
		uint32_t width = 10;
		uint32_t height = 40;
	};
}
