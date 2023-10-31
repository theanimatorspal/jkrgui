#include "Application.hpp"

void MainWindow_Draw(void* d)
{
	auto x = reinterpret_cast<ContextData*>(d);
	auto& m = *x;
	auto y = x->mWindow.GetWindowSize();

	m.mGUI2D.Draw(y.first, y.second);
	m.mNodeEditorScene.Draw(m.mGUI2D, y.first, y.second, m.mWindow);

	m.mLineR.DrawInit(m.mWindow);
	m.mNodeEditorScene.DrawConnections(m.mLineR, m.mWindow, y.first, y.second);

	m.mTextRenderer.Draw(y.first, y.second);
}

