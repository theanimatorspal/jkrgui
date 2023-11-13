#pragma once
#include <Components/Area_base.hpp>
#include <Components/ButtonRect_base.hpp>
#include <Components/Colors.hpp>
#include <Components/ScrollableRect.hpp>
#include <Components/HLayout.hpp>
#include <Components/VLayout.hpp>
#include <Components/BTextButtonRect.hpp>
#include <Components/TextLineEdit.hpp>
#include "GridSheet.hpp"


namespace App
{
	using namespace Jkr::Renderer;
	class DrawArea : public Component::Area_base
	{
		using ab = Component::Area_base;
	public:
		DrawArea(_2d& inR, EventManager& inE, Window* inWindow, uint32_t inW, uint32_t inH) : Component::Area_base(inR, inE)
		{
			this->SetWindow(inWindow, inW, inH);
			this->SetDimension(glm::uvec2(100, 100));
			this->SetPosition(
				glm::uvec2(
					this->GetWindowWidth() - this->GetDimension().x - 10,
					this->GetWindowHeight() - this->GetDimension().y - 10
				)
			);
			this->SetDepthValue(e.GetDepthValue());
			this->Load();

			/*1. Setup Layouts*/
			mVerticalLayout = MakeUp < Component::VerticalLayout<2>>(r, e);
			mVerticalLayout->SetPosition(glm::vec2(0, 0));
			mVerticalLayout->SetDimension(glm::vec2(this->GetWindowWidth(), this->GetWindowHeight()));

			mLayout = MakeSH<Component::HorizontalLayout<2>>(r, e);
			mBottomHorizontalLayout = MakeSH<Component::HorizontalLayout<1>>(r, e);
			mVerticalLayout->AddComponent(mLayout);
			mVerticalLayout->AddComponent(mBottomHorizontalLayout);
			mVerticalLayout->ResetPositionsAndDimensions({ 0.8f, 0.2f });

			mBottomScrollArea = MakeSH<Component::ScrollableRect>(r, e);
			mBottomHorizontalLayout->AddComponent(mBottomScrollArea);
			mBottomHorizontalLayout->ResetPositionsAndDimensions({ 1.0f });


			e.MoveDepthValueTowardsTheCamera();
			mGridSheet = MakeSH<App::GridSheet>(r, e);
			mScrollArea = MakeSH<Component::ScrollableRect>(r, e);
			mLayout->AddComponent(mScrollArea);
			mLayout->AddComponent(mGridSheet);
			mLayout->ResetPositionsAndDimensions({ 0.2f, 0.8f });

			/*2. Specify Settings For Components*/
			mGridSheet->SetDepthValue(e.GetDepthValue());
			mGridSheet->SetWindow(mWindow, this->GetWindowWidth(), this->GetWindowHeight());
			mScrollArea->SetDepthValue(e.GetDepthValue());
			mScrollArea->SetWindow(mWindow, this->GetWindowWidth(), this->GetWindowHeight());
			mScrollArea->SetVerticalScrollBarHeigthRatio(0.1f);
			mBottomScrollArea->SetDepthValue(e.GetDepthValue());
			mBottomScrollArea->SetWindow(mWindow, this->GetWindowWidth(), this->GetWindowHeight());
			mBottomScrollArea->SetVerticalScrollBarHeigthRatio(0.01f);

			/*3. Load the Components*/
			mGridSheet->Load();
			mScrollArea->Load();
			mBottomScrollArea->Load();
			e.MoveDepthValueAwayFromTheCamera();
			mLayout->SetVPadding(0);
			mLayout->SetHPadding(5);
			mVerticalLayout->SetVPadding(5);
			mVerticalLayout->SetHPadding(0);
			mBottomHorizontalLayout->SetVPadding(0);
			mBottomHorizontalLayout->SetHPadding(5);

			r.bt.AddFontFace("font.ttf", 4, mFontFaceId);
			r.bt.SetCurrentFontFace(mFontFaceId);
			mTextButton = MakeUp<Component::TextLineEdit>(r, e);
			//mTextButton->SetText("प्रथमोऽध्यायः");
			mTextButton->SetPosition(mScrollArea->FromComponent(glm::vec2(100, 100)));
			mTextButtonScrollTestPosition = mScrollArea->FromComponent(glm::vec2(100, 100));
			mTextButton->SetWindow(this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());
			mTextButton->SetDepthValue(e.GetDepthValue());
			mTextButton->Load();
		}
	private:
		Sp<Component::ScrollableRect> mScrollArea;
		Sp<App::GridSheet> mGridSheet;
		Sp<Component::HorizontalLayout<2>> mLayout;
		Sp<Component::HorizontalLayout<1>> mBottomHorizontalLayout;
		Sp<Component::ScrollableRect> mBottomScrollArea;
		Up<Component::VerticalLayout<2>> mVerticalLayout;
		Up<Component::TextLineEdit> mTextButton;
		uint32_t mFontFaceId;

	public:
		void Update(Window* inWindow, uint32_t inW, uint32_t inH)
		{
			this->SetWindow(inWindow, inW, inH);
			this->SetPosition(
				glm::uvec2(
					this->GetWindowWidth() - this->GetDimension().x - 10,
					this->GetWindowHeight() - this->GetDimension().y - 10
				)
			);

			mVerticalLayout->SetPosition(glm::vec2(0, 0));
			mVerticalLayout->SetDimension(glm::vec2(this->GetWindowWidth(), this->GetWindowHeight()));
			mVerticalLayout->ResetPositionsAndDimensions({ 0.8, 0.2 });


			mBottomHorizontalLayout->ResetPositionsAndDimensions({ 1.0f });
			mLayout->ResetPositionsAndDimensions({ 0.2, 0.8 });
			mScrollArea->Update(inWindow, this->GetWindowWidth(), this->GetWindowHeight());
			mBottomScrollArea->Update(inWindow, this->GetWindowWidth(), this->GetWindowHeight());
			mGridSheet->Update(inWindow, this->GetWindowWidth(), this->GetWindowHeight());



			//mTextButton->SetPosition(mScrollArea->FromComponent(glm::vec2(100, 100)));
			mTextButton->SetPosition(mScrollArea->GetScrollOffsetPosition() + mTextButtonScrollTestPosition);
			mTextButton->SetWindow(this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());
			mTextButton->Update();
		}
	private:
		glm::vec2 mTextButtonScrollTestPosition;

	public:
		void Event()
		{
			mScrollArea->Event();
			mGridSheet->Event();
			mBottomScrollArea->Event();
			mTextButton->Event();
		}

		void Draw()
		{
			r.ln.Bind(*mWindow);
			mScrollArea->DrawOutlines();
			mBottomScrollArea->DrawOutlines();
			mTextButton->DrawOutlines();

			/* GRIDSHEET */
			{
				mGridSheet->DrawOutline();
				mGridSheet->SetScissor();
				mGridSheet->DrawLines();
				mGridSheet->ResetScissor();
			}

			r.sh.BindShapes(*mWindow);
			r.sh.BindFillMode(Jkr::Renderer::FillType::Fill, *mWindow);
			mScrollArea->DrawFillShapes();
			mBottomScrollArea->DrawFillShapes();

			mScrollArea->SetScissor();
//			mTextButton->DrawShapes();

			r.bt.Bind(*this->GetWindow());
			mTextButton->DrawBestTexts();
			mScrollArea->ResetScissor();
		}

	};
}