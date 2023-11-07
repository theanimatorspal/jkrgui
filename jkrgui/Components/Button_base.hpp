#pragma once
#include "ConfigProps.hpp"
#include "Area_base.hpp"

namespace Jkr::Component {
	class Button_base : public Area_base
	{
		using ab = Area_base;
	public:
		Button_base(_2d& inR, EventManager& inE) : Area_base(inR, inE)
		{

		}
		constexpr void Load()
		{
			ab::Load();
			mBoundedRectId = e.SetBoundedRect(ab::GetPosition(), ab::GetDimension(), ab::GetDepthValue());
		}
		constexpr void Event()
		{
			Jkr::BoundRect2D Rect{ .mXy = ab::GetPosition(), .mWh = ab::GetDimension() };
			e.UpdateBoundRect(ab::GetDepthValue(), mBoundedRectId, Rect);
			if (e.IsMouseWithin(mBoundedRectId, ab::GetDepthValue()))
			{
				isBeingHovered = true;
				mCurrentColor = mButtonHoveredColor;
				if (e.GetMouseButtonValue() & SDL_BUTTON(SDL_BUTTON_LEFT))
				{
					mCurrentColor = mButtonClickedColor;
					isBeingPushed = true;
				}
				else {
					isBeingPushed = false;
				}
			}
			else {
				isBeingHovered = false;
				isBeingPushed = false;
				mCurrentColor = mButtonNormalColor;
			}
		}
		GETTER GetColor() { return mCurrentColor; }
		GETTER IsBeingPushed() { return isBeingPushed; }
		GETTER IsBeingHovered() { return isBeingHovered; }
	private:
		uint32_t mBoundedRectId;
		bool isBeingPushed = false;
		bool isBeingHovered = false;
		glm::vec4 mCurrentColor = glm::vec4(BUTTON_NORMAL, 1.0);
		glm::vec4 mButtonNormalColor = glm::vec4(BUTTON_NORMAL, 1.0);
		glm::vec4 mButtonHoveredColor = glm::vec4(BUTTON_HOVERED, 1.0);
		glm::vec4 mButtonClickedColor = glm::vec4(BUTTON_CLICK, 1.0);

	};
}
