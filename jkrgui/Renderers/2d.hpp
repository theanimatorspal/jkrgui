#pragma once
#include <Renderers/TwoD/Line.hpp>
#include <Renderers/TwoD/Shape.hpp>
#include <Renderers/TextD/BestText.hpp>

namespace Jkr::Renderer
{
	struct _2d
	{
        _2d(const Instance &inInstance,
            Window &inCompatibleWindow,
            ResourceManager &inResMan,
            uint32_t inVarDesCount = 5000)
            : ln(inInstance, inCompatibleWindow, inResMan.GetLineRendererCache())
            , sh(inInstance, inCompatibleWindow, inResMan.GetShapePainterCaches(), inVarDesCount)
            , bt(inInstance, inCompatibleWindow, inResMan.GetBestTextRendererCache(), inVarDesCount)
        {}

        Line ln;
        Shape sh;
        BestText bt;
	};
}
