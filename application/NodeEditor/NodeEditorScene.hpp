#pragma once
#include <Components/GridSheet.hpp>
#include <Renderers/TwoD/2d.hpp>
#include <EventManager.hpp>
#include "NodeProperties.hpp"

namespace App
{
	using namespace Jkr::Renderer;
	class NodeScene : protected Component::GridSheet
	{
		using gs = Jkr::Component::GridSheet;
	public:
		NodeScene(_2d& inRenderer2D, EventManager& inManager) : GridSheet(inRenderer2D, inManager) {
		}

		void Load(uint32_t inW, uint32_t inH)
		{
			ToWc towc(inW, inH);
			gs::SetGridSpacing(20);
			gs::SetNumLineFactor(2);
			gs::Load(inW, inH);
			mNodePropWindow = MakeUp<NodeProp>(r, e, inW, inH);
		}

		void BeginDraw() {
			gs::BeginDraw();
			mNodePropWindow->BeginDraw();
			transformMatrix = glm::translate(transformMatrix, glm::vec3(gs::mOffset2D, 0.0f)); 
		}
		void EndDraw() {
			gs::EndDraw();
			mNodePropWindow->EndDraw();
			transformMatrix = glm::identity<glm::mat4>(); 
		}

		void DrawLines(Window& inWindow, uint32_t inW, uint32_t inH)
		{
			gs::DrawLines(inWindow, inW, inH);
			mNodePropWindow->DrawLines(inWindow, inW, inH);
		}
		void DrawShapes(Window& inWindow, uint32_t inW, uint32_t inH)
		{
			gs::DrawShapes(inWindow, inW, inH);
			mNodePropWindow->DrawShapes(inWindow, inW, inH);
		}
		void DrawTexts(Window& inWindow, uint32_t  inW, uint32_t inH)
		{
		}
	private:
		Up<App::NodeProp> mNodePropWindow;
		glm::mat4 transformMatrix = glm::identity<glm::mat4>();
	public:
		void Dispatch()
		{
			gs::Dispatch();
		}
		void Event(Window& inWindow, uint32_t inW, uint32_t inH)
		{
			gs::Event(inWindow, inW, inH);
			mNodePropWindow->Event(inWindow, inW, inH);
		}
		void Update(uint32_t inW, uint32_t inH)
		{
			gs::Update(inW, inH);
		}

	};
}