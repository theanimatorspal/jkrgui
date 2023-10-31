#include "Application.hpp"

void MainWindow_Load(void* d)
{
	auto _x = reinterpret_cast<ContextData*>(d);
	auto& GUI2DRenderer = _x->mGUI2D;
	auto& MainWindow = _x->mWindow;
	auto& Font16 = _x->mTextRenderer;
	auto& MenuBar = _x->mMenuBar;
	auto& NodeEditorScene = _x->mNodeEditorScene;
	auto winD = MainWindow.GetWindowSize();

	MainWindow.SetUpdateCallBack(MainWindow_Update);
	MainWindow.SetDrawCallBack(MainWindow_Draw);
	MainWindow.SetComputeDispatchCallBack(MainWindow_Dispatch);
	MainWindow.SetResizeCallBack(MainWindow_Resize);
	MainWindow.SetMouseCallBack(MainWindow_EventCall);
}