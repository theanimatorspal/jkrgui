#pragma once
#include "Line.hpp"
#include "FastText.hpp"
#include "Shape.hpp"
#include "../ResourceManager.hpp"

namespace Jkr::Renderer
{
	struct _2d
	{
		_2d(const Instance& inInstance, Window& inCompatibleWindow, ResourceManager& inResMan)
			: ln(inInstance, inCompatibleWindow, inResMan.GetLineRendererCache()),
			sh(inInstance, inCompatibleWindow, inResMan.GetShapePainterCaches()),
			ft(inInstance, inCompatibleWindow, inResMan.GetFastTextRendererCache()) { }
		Line ln;
		Shape sh;
		FastText ft;
	};
}