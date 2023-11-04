#include "Button.hpp"

void Jkr::Component::Button::LoadShape()
{
    /*
    Set Before Loading
    */
    mBoundRectId = e.SetBoundedRect(mPos, mDimen, mDepthLevel);
    Jkr::Generator Rect(Jkr::Shapes::Rectangle, mDimen);
    r.sh.Add(Rect, 0, 0, mDepthLevel, mButonShapeId);
}

void Jkr::Component::Button::DrawShape(Window& inWindow, uint32_t inW, uint32_t inH, glm::mat4& inMatrix)
{
	auto matrix = glm::identity<glm::mat4>();
	matrix = glm::translate(matrix, glm::vec3(mPos, 0.0f));
	r.sh.Draw(inWindow, glm::vec4(mCurrentColor, 1.0f), inW, inH, mButonShapeId, mButonShapeId, matrix * inMatrix);
}

void Jkr::Component::Button::Event()
{
	if (e.IsMouseWithin(mBoundRectId, mDepthLevel))
	{
		mButtonHovered = true;
		mCurrentColor = BUTTON_HOVERED;
		if (e.GetMouseButtonValue() == SDL_BUTTON(1))
		{
			mCurrentColor = BUTTON_CLICK;
		}
	}
	else {
		mButtonHovered = false;
		mCurrentColor = BUTTON_NORMAL;
	}
}
