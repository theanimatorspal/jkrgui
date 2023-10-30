#pragma once
#define SDL_MAIN_HANDLED
#include "GUIMainWindow.hpp"
#include "GUIPrimitive.hpp"
#include "GUIPainterCache.hpp"
#include "GUIPainter.hpp"
#include "GUIPainterParameter.hpp"
#include <string>
#include <vector>

namespace Jkr {
	class GUIApp
	{
	public:
		GUIApp(GUIMainWindow& inWindow, std::string inAppName);
		inline void Run()
		{
			while (!mMainWindow.WindowShouldClose())
			{
				mMainWindow.PollEvents();
				mMainWindow.Draw();
			}
		}
	private:
		GUIMainWindow& mMainWindow;
		std::string mAppName;
		std::vector<GUIWindow> mWindows;
	};
}