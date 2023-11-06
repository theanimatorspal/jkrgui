#pragma once
#include "Button_base.hpp"

namespace Jkr::Component
{
	class ButtonRect_base : public Button_base
	{
		using bb = Button_base;
	public:
		ButtonRect_base(_2d& inR, EventManager& inE) : Button_base(inR, inE)
		{ }
		void Load()
		{
			bb::Load();
			Jkr::Generator Rect(Jkr::Shapes::Rectangle, bb::GetDimension());
			uint32_t id;
			r.sh.Add(Rect, bb::GetPosition().x, bb::GetPosition().y, bb::GetDepthValue(), id);
			bb::GetShapes().push_back(glm::uvec2(id, id));
		}
	};
}