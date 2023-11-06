#include "Button.hpp"

void Jkr::Component::Button::BeginDraw()
{
	mTmatrix = glm::identity<glm::mat4>();
	mTmatrix = glm::translate(mTmatrix, glm::vec3(cb::GetPosition(), 0.0f));
}

void Jkr::Component::Button::EndDraw()
{
	mTmatrix = glm::identity<glm::mat4>();
}

void Jkr::Component::Button::LoadShape()
{
    mBoundRectId = e.SetBoundedRect(cb::GetPosition(), cb::GetDimension(), cb::GetDepthValue());
    Jkr::Generator Rect(Jkr::Shapes::Rectangle, cb::GetDimension());
    r.sh.Add(Rect, 0, 0, cb::GetDepthValue(), mButonShapeId);
}

void Jkr::Component::Button::DrawShape(Window& inWindow, uint32_t inW, uint32_t inH, glm::mat4& inMatrix)
{
	auto newPos = inMatrix * glm::vec4(cb::GetPosition().x, cb::GetPosition().y, 0.0f, 1.0f);
	Jkr::BoundRect2D rect = { .mXy = newPos, .mWh = cb::GetDimension()};
	e.UpdateBoundRect(cb::GetDepthValue(), mBoundRectId, rect);
	r.sh.Draw(inWindow, glm::vec4(mCurrentColor, 1.0f), inW, inH, mButonShapeId, mButonShapeId, mTmatrix * inMatrix);
}

void Jkr::Component::Button::Event()
{
	if (e.IsMouseWithin(mBoundRectId, cb::GetDepthValue()))
	{
		mButtonHovered = true;
		mCurrentColor = mButtonHoveredColor;
		if (e.GetMouseButtonValue() & SDL_BUTTON(SDL_BUTTON_LEFT))
		{
			mCurrentColor = mClickColor;
			mButtonPushed = true;
		}
		else {
			mButtonPushed = false;
		}
	}
	else {
		mButtonHovered = false;
		mButtonPushed = false;
		mCurrentColor = mNormalColor;
	}

}
