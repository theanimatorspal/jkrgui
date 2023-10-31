#include "Application.hpp"

void MainWindow_Update(void* d)
{
	auto x = reinterpret_cast<ContextData*>(d);
	auto& GUI2DRenderer = x->mGUI2D;
	auto& MainWindow = x->mWindow;
	auto& Font16 = x->mTextRenderer;
	auto& MenuBar = x->mMenuBar;
	auto& NodeEditorScene = x->mNodeEditorScene;
	auto winD = MainWindow.GetWindowSize();
	MenuBar.Update(winD.first, winD.second);
	NodeEditorScene.Update(GUI2DRenderer, 0, MenuBar.GetMenuBarHeight(), winD.first * 0.7, winD.second - MenuBar.GetMenuBarHeight());
	NodeEditorScene.UpdateConnections(x->mLineR, MainWindow, winD.first, winD.second);
}

void MainWindow_Resize(void* d)
{
	auto x = reinterpret_cast<ContextData*>(d);
	auto& GUI2DRenderer = x->mGUI2D;
	auto& MainWindow = x->mWindow;
	auto& Font16 = x->mTextRenderer;
	auto& MenuBar = x->mMenuBar;
	auto winD = MainWindow.GetWindowSize();
	MenuBar.UpdateResized(winD.first, winD.second);
}

void MainWindow_EventCall(std::any inData, std::any inMouse)
{
	auto xv = std::any_cast<void*>(inData);
	auto x = static_cast<ContextData*>(xv);
	auto& GUI2DRenderer = x->mGUI2D;
	auto& MainWindow = x->mWindow;
	auto& Font16 = x->mTextRenderer;
	auto& MenuBar = x->mMenuBar;
	auto& NodeEditorScene = x->mNodeEditorScene;
	auto winD = MainWindow.GetWindowSize();
	MenuBar.EventCall(MainWindow, inMouse);
	NodeEditorScene.EventCall(x->mLuaConfig, MainWindow, x->mLineR, inMouse);
}

