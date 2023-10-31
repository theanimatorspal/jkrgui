#pragma once
#include "GUIApplication.hpp"
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "FastTextFrontEnd.hpp"
#include "GUI2DRenderer.hpp"
#include "AllShaders.hpp"
#include "MenuBar.hpp"
#include "EventManager.hpp"
#include "NodeEditor/NodeEditorScene.hpp"
#include "GeneralRenderers/Line.hpp"


struct ContextData {
	const Jkr::Instance& mInstance;
	Jkr::GUIMainWindow& mWindow;
	Jkr::Font::FastTextRenderer& mTextRenderer;
	Jkr::GUI2DRenderer& mGUI2D;
	MenuBar& mMenuBar;
	App::NodeEditorScene& mNodeEditorScene;
	Jkr::Renderer::Line& mLineR;
	App::LuaConfig& mLuaConfig;
};

void MainWindow_Dispatch(void* d);
void MainWindow_Update(void* d);
void MainWindow_Draw(void* d);
void MainWindow_Load(void* d);
void MainWindow_Resize(void* d);
void MainWindow_EventCall(std::any inData, std::any inMouse);

void LoadCaches(Jkr::GUIPainterCache& FontPainterCache, Jkr::GUIPainterCache& GUI2DPainterCache, Jkr::GUIPainterCache& LineRendererCache);
