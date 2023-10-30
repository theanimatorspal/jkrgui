#pragma once
#include "GUIWindow.hpp"
namespace Jkr {
	class GUIMainWindow : public GUIWindow
	{
	public:
		GUIMainWindow(const Instance& inInstance, std::string inTitle, int inHeight, int inWidth);
	};
}
