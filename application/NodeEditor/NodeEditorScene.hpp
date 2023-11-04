#pragma once
#include <Components/GridSheet.hpp>
namespace App
{
	class NodeScene : protected Jkr::Component::GridSheet
	{
		using cb = Component_base;
	public:
		GridSheet(_2d& inRenderer2D, EventManager& inManager) : r(inRenderer2D), e(inManager) { }
		glm::uvec2 ToWc(uint32_t inW, uint32_t inH, glm::uvec2 inC);
		void Load(uint32_t inW, uint32_t inH);
		void Update();
		void DrawLines(Window& inWindow, uint32_t inW, uint32_t inH);
		void DrawShapes(Window& inWindow, uint32_t inW, uint32_t inH);
		void Dispatch();
		void Event();
	private:
		_2d& r;
		EventManager& e;
	};
}