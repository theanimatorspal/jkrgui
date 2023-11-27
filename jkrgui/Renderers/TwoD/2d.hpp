#pragma once
#include "Line.hpp"
#include "FastText.hpp"
#include "Shape.hpp"
#include "BestText.hpp"
#include "../ResourceManager.hpp"

namespace Jkr::Renderer
{
	struct _2d
	{
		_2d(const Instance& inInstance, Window& inCompatibleWindow, ResourceManager& inResMan)
			: ln(inInstance, inCompatibleWindow, inResMan.GetLineRendererCache()),
			sh(inInstance, inCompatibleWindow, inResMan.GetShapePainterCaches()),
			bt(inInstance, inCompatibleWindow, inResMan.GetBestTextRendererCache()) { }

		Line ln;
		Shape sh;
		BestText bt;
	};
}