#pragma once
#include "Scrollable_base.hpp"
#include "ButtonRect_base.hpp"
#include "FocusProxy.hpp"
#ifdef max
#undef max
#endif

namespace Jkr::Component
{
	class ScrollableRect : public Scrollable_base, public FocusProxy
	{
		using sb = Scrollable_base;
	public:
		ScrollableRect(_2d& inR, EventManager& inE) : Scrollable_base(inR, inE)
		{ 
			FocusProxy::SetFocusOnHover(true);
		}
		void Load()
		{
			sb::Load();
			this->SetDefaultBoundedRectangle();
			mScrollBarHeight = sb::GetVerticalScrollBarHeigthRatio() * sb::GetDimension().y;
			e.MoveDepthValueTowardsTheCamera();

			mVerticalScrollBarButton = MakeUp<ButtonRect_base>(r, e);
			mVerticalScrollBarButton->SetDimension(glm::uvec2(mScrollBarWidth, mScrollBarHeight));
			mVerticalScrollBarButton->SetDepthValue(sb::GetDepthValue());
			mVerticalScrollBarButton->SetWindow(this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());
			mVerticalScrollBarButton->SetPosition(
				this->FromComponent(
					glm::vec2(sb::GetDimension().x - mScrollBarWidth,
						(sb::GetDimension().y - mScrollBarHeight) * sb::GetNormalizedValue()
					))
			);
			mVerticalScrollBarButton->SetDepthValue(e.GetDepthValue());
			mVerticalScrollBarButton->Load();
		}
		void Update(Window* inWindow, uint32_t inW, uint32_t inH)
		{
			Area_base::Update();
			this->UpdateDefaultBoundedRectangle();
			mVerticalScrollBarButton->SetPosition(
				this->FromComponent(
					glm::vec2(sb::GetDimension().x - mScrollBarWidth,
						(sb::GetDimension().y - mScrollBarHeight) * this->GetNormalizedValue()
					))
			);
			this->SetWindow(inWindow, inW, inH);
		}
		void DrawOutlines()
		{
			for (auto u : this->GetLines())
			{
				r.ln.Draw(*mWindow, FocusProxy::GetFocusColor(), sb::GetWindowWidth(), sb::GetWindowHeight(), this->GetLines()[0].x, this->GetLines()[0].y, this->GetTranslationMatrix());
			}
		}
		void DrawFillShapes()
		{
			for (auto u : mVerticalScrollBarButton->GetShapes())
			{
				r.sh.Draw(*mWindow, mVerticalScrollBarButton->GetColor(), sb::GetWindowWidth(), sb::GetWindowHeight(), u.x, u.y, mVerticalScrollBarButton->GetTranslationMatrix());
			}
		}
		void Event()
		{
			mVerticalScrollBarButton->Event();
			int numKeys;
			auto keystate = SDL_GetKeyboardState(&numKeys);

			bool isLeftButtonPressed = (SDL_BUTTON(SDL_BUTTON_LEFT) bitand e.GetMouseButtonValue()) != 0;
			bool isVerticalButtonPushed = mVerticalScrollBarButton->IsBeingPushed();
			bool isMouseOver = this->IsMouseWithin();
			bool isMouseOnTop = this->IsMouseOnTop();

			bool shouldVScroll = isLeftButtonPressed and isVerticalButtonPushed;

			if (shouldVScroll) mVerticalScrollBarBeingScrolled = true;
			if (!isLeftButtonPressed) mVerticalScrollBarBeingScrolled = false;

			if (mVerticalScrollBarBeingScrolled || shouldVScroll)
			{
				auto vButtonPos = mVerticalScrollBarButton->GetPosition();
				auto vButtonDim = mVerticalScrollBarButton->GetDimension();
				auto cbPos = sb::GetPosition();
				auto cbDim = sb::GetDimension();
				vButtonPos.y += e.GetRelativeMousePos().y;
				vButtonPos.y = std::max(cbPos.y, std::min(vButtonPos.y, cbPos.y + cbDim.y - vButtonDim.y));
				mVerticalScrollBarButton->SetPosition(glm::vec2(mVerticalScrollBarButton->GetPosition().x, vButtonPos.y));
				this->SetNormalizedValue((vButtonPos.y - cbPos.y) / (cbDim.y - vButtonDim.y));
			}


			FocusProxy::FocusEvent(*this, isLeftButtonPressed, isMouseOver, isMouseOnTop);

			if (e.GetEventHandle().type == SDL_MOUSEWHEEL and this->IsFocused())
			{
				if (e.GetEventHandle().wheel.y > 0)
				{
					this->SetNormalizedValue(this->GetNormalizedValue() - 0.01);
				}
				else if (e.GetEventHandle().wheel.y < 0)
				{
					this->SetNormalizedValue(this->GetNormalizedValue() + 0.01);
				}
			}
		}
		GETTER GetScissor()
		{
			return vk::Rect2D(
				vk::Offset2D(this->GetPosition().x, this->GetPosition().y),
				vk::Extent2D(this->GetDimension().x - mVerticalScrollBarButton->GetDimension().x, this->GetDimension().y)
			);
		}
        void SetScissor() { mWindow->SetScissor(GetScissor()); }
        void ResetScissor() { mWindow->ResetScissor(); }
        GETTER GetScrollOffsetPosition()
		{
			return glm::vec2(0.0f, -mVerticalScrollBarButton->GetPosition().y + this->GetPosition().y);
		}
	private:
		glm::vec4 mBackGroundColor = glm::vec4(color_SMOKEYTOPAZ, 1.0);
		bool mVerticalScrollBarBeingScrolled = false;
		Up<ButtonRect_base> mVerticalScrollBarButton;
		uint32_t mScrollBarWidth = 10;
		uint32_t mScrollBarHeight = 10;
	};
}
