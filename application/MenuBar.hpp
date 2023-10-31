#pragma once
#include "GUIWindow.hpp"
#include "GUI2DRenderer.hpp"
#include "FastTextFrontEnd.hpp"
#include "Button.hpp"
#include "TextButton.hpp"
#include "LuaConfig.hpp"
#include "EventManager.hpp"


class MenuBar
{
public:
	MenuBar(Jkr::GUI2DRenderer& inRenderer, Jkr::Font::FastTextRenderer& inTextRenderer, App::LuaConfig& inManager, App::EventManager& inEventManager)
		: mRenderer(inRenderer), mTextRenderer(inTextRenderer), mLuaManager(inManager),
		mFileMenuMaximizeButton(mRenderer, mTextRenderer),
		mFileMenuMinimizeButton(mRenderer, mTextRenderer),
		mFileMenuCloseButton(mRenderer, mTextRenderer),
		mEventManager(inEventManager)
	{
	}
	void EventCall(Jkr::GUIWindow& inWindow, std::any inEvent);
	void LoadConfigurationFile(const std::string inConfigFile);
	void UpdateResized(uint32_t inWindowWidth, uint32_t inWindowHeight);
	void Dispatch();
	void Update(uint32_t inWindowWidth, uint32_t inWindowHeight);
	void Register(uint32_t inWindowWidth, uint32_t inWindowHeight);
	GETTER GetMenuBarHeight() const { return mSizeOfTitleBox; }
private:
	Jkr::GUI2DRenderer& mRenderer;
	Jkr::Font::FastTextRenderer& mTextRenderer;
	ksai::kstd::ui32 mFileMenuBackGroundId;
	ksai::kstd::ui32 mFileMenuBackGroundShadowId;
	Button mFileMenuMaximizeButton;
	Button mFileMenuMinimizeButton;
	Button mFileMenuCloseButton;
	std::vector<std::vector<TextButton>> mFileMenuItems;
private:
	App::LuaConfig& mLuaManager;
	glm::vec3 mColorOfTheBar = glm::vec3(0.3f, 0.3f, 0.3f);
	glm::vec3 mColorOfTheThreeButtonsBg = glm::vec3(0.8f, 0.8f, 0.8f);
	ksai::kstd::ui32 mSizeOfTitleBox = 25;
private:
	ksai::kstd::ui32 mTextComponentDepthValue = 1;
	ksai::kstd::ui32 mButtonComponentDepthValue = 2;
	ksai::kstd::ui32 mBarBackgroundDepthValue = 3;
	int mMenuBarOnIndex = -1;
private:
	App::EventManager& mEventManager;
};


