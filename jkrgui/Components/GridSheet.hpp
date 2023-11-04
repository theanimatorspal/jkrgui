#pragma once
#include <Renderers/TwoD/2d.hpp>
#include "Component_base.hpp"
#include "EventManager.hpp"
#include "FastTextButton.hpp"

namespace Jkr::Component
{
	using namespace Jkr::Renderer;
	class GridSheet : public Component_base
	{
		using cb = Component_base;
	public:
		GridSheet(_2d& inRenderer2D, EventManager& inManager) : r(inRenderer2D), e(inManager) { }
		void Load(uint32_t inW, uint32_t inH);
		void Update(uint32_t inW, uint32_t inH);
		void DrawLines(Window& inWindow, uint32_t inW, uint32_t inH);
		void DrawShapes(Window& inWindow, uint32_t inW, uint32_t inH);
		void Dispatch();
		void Event(Window& inWindow, uint32_t inW, uint32_t inH);
	protected:
		_2d& r;
		EventManager& e;
		SETTER SetGridSpacing(uint32_t inSpacing) { mGridSpacing = inSpacing; }
	private:
		uint32_t mDepthValue;
		uint32_t mGridSpacing = 10;
		uint32_t mGridStartId;
		uint32_t mGridEndId;
		uint32_t mGridMainLinesId;
		uint32_t mCenterIndicatorId;
		glm::vec2 mOffset2D = { 0, 0 };
		glm::vec2 mPreviousOffset = { 0, 0 };
		glm::mat4 mTmatrix = glm::identity<glm::mat4>();
	protected:
		class ToWc
		{
		public:
			constexpr ToWc(uint32_t inW, uint32_t inH) : mW(inW), mH(inH) {}
			constexpr glm::vec2 operator()(glm::vec2 inC) {
				return inC + glm::vec2(mW / 2.f, mH / 2.f);
			}
		private:
			uint32_t mW, mH;
		};
		class TogWc
		{
		public:
			constexpr TogWc(uint32_t inW, uint32_t inH) : mW(inW), mH(inH) {}
			constexpr glm::vec2 operator()(glm::vec2 inC) {
				return inC - glm::vec2(mW / 2.f, mH / 2.f);
			}
		private:
			uint32_t mW, mH;
		};
	};
}
