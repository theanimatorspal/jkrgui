#pragma once
#include <Components/ScrollableProxy.hpp>

namespace App
{
	using namespace Jkr::Renderer;
	class NodeProp : protected Component::ScrollableProxy
	{
		using sp = Component::ScrollableProxy;
	public:
		NodeProp(_2d& inRenderer2D, EventManager& inManager, uint32_t inW, uint32_t inH) : sp(inRenderer2D, inManager) { 
			sp::SetOffset(glm::vec2(0, 0));
			sp::SetDimension(glm::uvec2(width, height));
			//sp::SetPosition(glm::vec2(inW - width, inH - height));
			sp::SetPosition(glm::vec2(0, 0));
			e.MoveDepthValueTowardsTheCamera();
			e.MoveDepthValueTowardsTheCamera();
			sp::SetDepthValue(e.GetDepthValue());
			sp::Load(inW, inH);

			e.MoveDepthValueTowardsTheCamera();

			mStartLineId = r.ln.GetCurrentNewLineId();
			for (int i = 0; i < 900; i++)
			{
				r.ln.AddLine(glm::vec2(0, i * 20), glm::vec2(inW, i * 20), e.GetDepthValue(), mEndLineId);
			}
			e.MoveDepthValueAwayFromTheCamera();
		}
		constexpr void BeginDraw() {
			sp::BeginDraw();
			mTransform = mTransform * sp::GetClippingMatrix();
		}
		constexpr void EndDraw() {
			sp::EndDraw();
		}

		constexpr void Load(uint32_t inW, uint32_t inH) {
			sp::Load(inW, inH);
		}
		constexpr void Update(uint32_t inW, uint32_t inH)
		{
		}
		constexpr void DrawLines(Window& inWindow, uint32_t inW, uint32_t inH) {
			r.ln.Draw(inWindow, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), inW, inH, mStartLineId, mEndLineId, mTransform);
		}
	private:
		glm::mat4 mTransform = glm::identity<glm::mat4>();
		uint32_t mStartLineId;
		uint32_t mEndLineId;
		uint32_t width = 300;
		uint32_t height = 200;

	public:
		constexpr void DrawShapes(Window& inWindow, uint32_t inW, uint32_t inH)
		{
			sp::DrawShapes(inWindow, inW, inH);
		}
		constexpr void Dispatch() {}
		constexpr void Event(Window& inWindow, uint32_t inW, uint32_t inH)
		{
			sp::Event(inWindow, inW, inH);
			sp::SetDimension(glm::uvec2(width, height));
			sp::SetPosition(glm::vec2(inW - width, inH - height));
		}
	};

};
