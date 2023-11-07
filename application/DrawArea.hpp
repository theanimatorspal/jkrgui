#pragma once
#include <Components/Area_base.hpp>
#include <Components/ButtonRect_base.hpp>
#include <Components/Colors.hpp>
#include <Components/ScrollableRect.hpp>
#include <Components/VLayout.hpp>
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


			mLayout = MakeUp<Component::HorizontalLayout>(r, e);
			mLayout->SetPosition(glm::vec2(0, 0));
			mLayout->SetDimension(glm::vec2(this->GetWindowWidth(), this->GetWindowHeight()));

			e.MoveDepthValueTowardsTheCamera();
			mGridSheet = MakeSH<App::GridSheet>(r, e);
			mGridSheet->SetDepthValue(e.GetDepthValue());
			mGridSheet->SetWindow(mWindow, this->GetWindowWidth(), this->GetWindowHeight());


			mScrollArea = MakeSH<Component::ScrollableRect>(r, e);
			mScrollArea->SetDepthValue(e.GetDepthValue());
			mScrollArea->SetWindow(mWindow, this->GetWindowWidth(), this->GetWindowHeight());
			//mScrollArea->SetDimension(glm::uvec2(100, 100));
			//mScrollArea->SetPosition(glm::uvec2(400, 400));

			mLayout->AddComponent(mScrollArea);
			mLayout->AddComponent(mGridSheet);
			mLayout->ResetPositionsAndDimensions();

			mGridSheet->Load();
			mScrollArea->Load();
			e.MoveDepthValueAwayFromTheCamera();
		}
	private:
		Sp<Component::ScrollableRect> mScrollArea;
		Sp<App::GridSheet> mGridSheet;
		Up<Component::HorizontalLayout> mLayout;

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
			mLayout->SetPosition(glm::vec2(0, 0));
			mLayout->SetDimension(glm::vec2(this->GetWindowWidth(), this->GetWindowHeight()));
			mLayout->ResetPositionsAndDimensions();
			mScrollArea->SetWindow(inWindow, this->GetWindowWidth(), this->GetWindowHeight());
			mScrollArea->Update();
		}

		void Event()
		{
			mScrollArea->Event();
			mGridSheet->Event();

		}

		void Draw()
		{
			r.ln.Bind(*mWindow);
			mScrollArea->DrawOutlines();
			mGridSheet->DrawLines();

			r.sh.BindShapes(*mWindow);
			r.sh.BindFillMode(Jkr::Renderer::FillType::Fill, *mWindow);
			mScrollArea->DrawFillShapes();
		}

	};
}