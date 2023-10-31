#include "Application.hpp"

void MainWindow_Dispatch(void* d)
{
	auto x = reinterpret_cast<ContextData*>(d);
	auto& GUI2DRenderer = x->mGUI2D;
	auto& MainWindow = x->mWindow;
	auto& Font16 = x->mTextRenderer;
	auto& MenuBar = x->mMenuBar;
	auto Time = x->mWindow.GetWindowCurrentTime();
	auto& NodeEditorScene = x->mNodeEditorScene;

	x->mTextRenderer.Dispatch();
	x->mGUI2D.Dispatch(Time);
	x->mLineR.Dispatch(x->mWindow);
	MenuBar.Dispatch();
}
