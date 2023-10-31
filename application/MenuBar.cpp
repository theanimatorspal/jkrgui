#include "MenuBar.hpp"

void MenuBar::EventCall(Jkr::GUIWindow& inWindow, std::any inEvent)
{
	auto Event = std::any_cast<SDL_Event*>(inEvent);
	struct {
		int x, y;
	} Mouse;
	auto MouseState = SDL_GetMouseState(&Mouse.x, &Mouse.y);
	struct {
		int w, h;
	} Window;
	auto wd = inWindow.GetWindowSize();
	Window.w = wd.first;
	Window.h = wd.second;
	/*
	CloseButton
	*/
	mFileMenuCloseButton.EventCall(inWindow, inEvent);
	mFileMenuMaximizeButton.EventCall(inWindow, inEvent);
	mFileMenuMinimizeButton.EventCall(inWindow, inEvent);
	static bool ShouldUpdateMenuBar = true;
	static bool MenuOn = false;
	for (int i = 0; i < mFileMenuItems.size(); i++)
	{
		for (int j = 0; j < mFileMenuItems[i].size(); j++)
		{
			auto& Button = mFileMenuItems[i][j];
			Button.EventCall(inWindow, inEvent);
			if (Button.GetIsButtonPushed() && j == 0)
			{
				if (!MenuOn)
				{
					mMenuBarOnIndex = i;
					MenuOn = true;
				}
				else {
					mMenuBarOnIndex = -1;
					MenuOn = false;
				}
				ShouldUpdateMenuBar = true;
			}
			else if (Button.GetIsButtonPushed() && j != 0) {
				mMenuBarOnIndex = -1;
				ShouldUpdateMenuBar = true;
				mEventManager.FireEvent(App::EventManager::Type::MenuBarClick, glm::ivec2(i, j));
				MenuOn = false;
			}
			else if (Button.GetIsButtonPushed())
			{
				MenuOn = false;
			}
		}
	}
	if (mFileMenuCloseButton.GetIsButtonPushed()) inWindow.SetWindowShouldClose(true);

	if (mFileMenuMaximizeButton.GetIsButtonPushed()) {
		inWindow.ToggleWindowFullScreen();
	}

	if (mFileMenuMinimizeButton.GetIsButtonPushed())
	{
		inWindow.Minimize();
	}

	if (ShouldUpdateMenuBar)
	{
		uint32_t PreviousTextWidth = 0;
		for (int i = 0; i < mFileMenuItems.size(); i++)
		{
			for (int j = 0; j < mFileMenuItems[i].size(); j++)
			{
				auto& Button = mFileMenuItems[i][j];
				if (j == 0) Button.Update(false);
				else if (j != 0 && i == mMenuBarOnIndex) Button.Update(false);
				else Button.Update(true);
			}
		}
		ShouldUpdateMenuBar = false;
	}
}





void MenuBar::LoadConfigurationFile(const std::string inConfigFile)
{
	mLuaManager.ExecuteFile(inConfigFile.c_str());
	mColorOfTheBar = mLuaManager.GetGlobal<glm::vec3>((char*)"MenuBar_BarColor");
	mColorOfTheThreeButtonsBg = mLuaManager.GetGlobal<glm::vec3>((char*)"MenuBar_ThreeButtonBackground");
	using MenuItemsType = std::vector<std::vector<std::string>>;
	auto MenuItems = mLuaManager.GetGlobal<MenuItemsType>((char*)"MenuBar_Items");

	{
		for (const auto& u : MenuItems)
		{
			mFileMenuItems.push_back(std::vector<TextButton>());
		}

		uint32_t PreviousTextWidth = 0;
		for (int i = 0; i < mFileMenuItems.size(); i++)
		{
			uint32_t WidthOfFirstElement = 0;
			uint32_t PreviousTextHeight = 0;
			for (int j = 0; j < MenuItems[i].size(); j++) {
				TextButton Button(mRenderer, mTextRenderer);
				auto Dimensions = Button.Register(MenuItems[i][j].c_str(), PreviousTextWidth, PreviousTextHeight, mSizeOfTitleBox, mColorOfTheBar);
				mFileMenuItems[i].push_back(Button);
				WidthOfFirstElement += (j == 0) ? Dimensions.mWidth : 0;
				PreviousTextWidth += (j == MenuItems[i].size() - 1) ? WidthOfFirstElement : 0;
				PreviousTextHeight += mSizeOfTitleBox;
			}
			PreviousTextHeight = 0;
		}
	}
}






void MenuBar::UpdateResized(uint32_t inWindowWidth, uint32_t inWindowHeight)
{
	mRenderer.UpdateDynamicQuad(mFileMenuBackGroundId, 0, 0, inWindowWidth - mSizeOfTitleBox, mSizeOfTitleBox, 5);
	mRenderer.UpdateDynamicQuad(mFileMenuBackGroundShadowId, 0, 2, inWindowWidth, mSizeOfTitleBox, 6);
	mFileMenuMaximizeButton.Update(inWindowWidth - mSizeOfTitleBox * 3, 0, mSizeOfTitleBox, mSizeOfTitleBox, 3);
	mFileMenuMinimizeButton.Update(inWindowWidth - mSizeOfTitleBox * 2, 0, mSizeOfTitleBox, mSizeOfTitleBox, 3);
	mFileMenuCloseButton.Update(inWindowWidth - mSizeOfTitleBox * 1, 0, mSizeOfTitleBox, mSizeOfTitleBox, 3);
}






void MenuBar::Dispatch()
{
	mFileMenuCloseButton.Dispatch<Button::Type::CircleInRectangle>(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.0f, 0.0f));
	mFileMenuMinimizeButton.Dispatch<Button::Type::CircleInRectangle>(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.5f, 0.0f));
	for (int i = 0; i < mFileMenuItems.size(); i++)
	{
		for (int j = 0; j < mFileMenuItems[i].size(); j++)
		{
			auto& Button = mFileMenuItems[i][j];
			Button.Dispatch<Button::Type::Rectangle>(mColorOfTheBar, mColorOfTheBar / 2.0f);
		}
	}
}






void MenuBar::Update(uint32_t inWindowWidth, uint32_t inWindowHeight)
{
}






void MenuBar::Register(uint32_t inWindowWidth, uint32_t inWindowHeight)
{

	mRenderer.RegisterDynamicQuad(mFileMenuBackGroundId, [=](Jkr::GUI2DRenderer& inG, ksai::kstd::ui32 inId)
		{
			inG.DrawToQuadTextureDispatch(
				inId, Jkr::GUI2DRenderer::Shape::Fill,
				glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
				glm::vec4(mColorOfTheBar, 1.0f),
				glm::vec4(0.0f)
			);
		}, 100, 100
	);
	mRenderer.RegisterDynamicQuad(mFileMenuBackGroundShadowId,
		[=](Jkr::GUI2DRenderer& inG, ksai::kstd::ui32 inId)
		{
			inG.DrawToQuadTextureDispatch(
				inId, Jkr::GUI2DRenderer::Shape::Fill,
				glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
				glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
				glm::vec4(0.0f)
			);
		}, 1, 1

	);
	mFileMenuCloseButton.Register<Button::Type::CircleInRectangle>(0, 0, 100, 100, mColorOfTheThreeButtonsBg, glm::vec3(1.0f, 0.0f, 0.0f), 0.3, 0.5f);
	mFileMenuMinimizeButton.Register<Button::Type::CircleInRectangle>(0, 0, 100, 100, mColorOfTheThreeButtonsBg, glm::vec3(0.0f, 1.0f, 0.0f), 0.3, 0.5f);
	mFileMenuMaximizeButton.Register<Button::Type::CircleInRectangle>(0, 0, 100, 100, mColorOfTheThreeButtonsBg, glm::vec3(0.0f, 0.0f, 1.0f), 0.3, 0.5f);

	UpdateResized(inWindowWidth, inWindowHeight);
}
